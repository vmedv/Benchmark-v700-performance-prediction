package contention.benchmark.ThreadLoops;

import contention.abstractions.CompositionalMap;
import contention.abstractions.KeyGenerator;
import contention.abstractions.ThreadLoopAbstract;
import contention.benchmark.ThreadLoops.parameters.DefaultThreadLoopParameters;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * The loop executed by each thread of the map
 * benchmark.
 *
 * @author Vincent Gramoli
 */
public class ThreadMapLoop extends ThreadLoopAbstract {

    /**
     * The instance of the running benchmark
     */
    public CompositionalMap<Integer, Integer> bench;
    /**
     * The pool of methods that can run
     */
    protected Method[] methods;

    /**
     * The random number
     */
    protected Random rand = new Random();

    /**
     * The distribution of methods as an array of percentiles
     * <p>
     * 0%        cdf[0]        cdf[2]                     100%
     * |--writeAll--|--writeSome--|--readAll--|--readSome--|
     * |-----------write----------|--readAll--|--readSome--| cdf[1]
     */
    private double[] cdf = new double[4];

    public ThreadMapLoop(short myThreadNum, CompositionalMap<Integer, Integer> bench, Method[] methods,
                         KeyGenerator keygen, DefaultThreadLoopParameters parameters) {
        super(myThreadNum, methods, keygen);
        this.bench = bench;
        /* initialize the method boundaries */
        assert (parameters.numWrites >= parameters.numWriteAlls);
        cdf[0] = parameters.numWriteAlls;
        cdf[1] = parameters.numInsert;
        cdf[2] = cdf[1] + parameters.numRemove;
        cdf[3] = cdf[2] + parameters.numSnapshots;
    }

    public void printDataStructure() {
        System.out.println(bench.toString());
    }

    @Override
    public void run() {

        while (!stop) {
            Integer a, b;
            double coin = rand.nextDouble();
            if (coin < cdf[0]) { // 1. should we run a writeAll operation?
                int newInt = keygen.nextGet();

                // init a collection
                Map<Integer, Integer> hm = new HashMap<Integer, Integer>(newInt, newInt);
                hm.put(newInt / 2, newInt / 2); // accepts duplicate

                try {
                    // todo something very strange
                    if (bench.keySet().removeAll(hm.keySet()))
                        numRemoveAll++;
                    else failures++;
                } catch (Exception e) {
                    System.err.println("Unsupported writeAll operations! Leave the default value of the numWriteAlls parameter (0).");
                }


            } else if (coin < cdf[1]) { // 2. should we run an insert

                int newInt = keygen.nextInsert();

                if ((a = bench.putIfAbsent(newInt, newInt)) == null) {
                    numAdd++;
                } else {
                    failures++;
                }
            } else if (coin < cdf[2]) { // 3. should we run a remove

                int newInt = keygen.nextRemove();

                if ((a = bench.remove(newInt)) != null) {
                    numRemove++;
                } else {
                    failures++;
                }

            } else if (coin < cdf[3]) { // 4. should we run a readAll operation?

                bench.size();
                numSize++;

            } else { //if (coin < cdf[3]) { // 5. then we should run a readSome operation
                int newInt = keygen.nextGet();
                if (bench.get(newInt) != null)
                    numContains++;
                else
                    failures++;
            } //else {
//                sleep(Parameters.sleepTime);
            // warmup для определения sleepTime
//            }
            total++;

            assert total == failures + numContains + numSize + numRemove
                    + numAdd + numRemoveAll + numAddAll;
        }
        // System.out.println(numAdd + " " + numRemove + " " + failures);
        this.getCount = CompositionalMap.counts.get().getCount;
        this.nodesTraversed = CompositionalMap.counts.get().nodesTraversed;
        this.structMods = CompositionalMap.counts.get().structMods;
        System.out.println("Thread #" + myThreadNum + " finished.");
    }

    @Override
    public void prefill(AtomicInteger prefillSize) {
        while (prefillSize.get() > 0) {
            int curPrefillSize = prefillSize.decrementAndGet();
            int v = keygen.nextPrefill();
            try {
            if (curPrefillSize < 0 || bench.putIfAbsent(v, v) != null) {
                prefillSize.incrementAndGet();
            }} catch (NullPointerException e) {
                System.out.println(v);
                System.out.println(curPrefillSize);
                throw e;
            }
        }
    }

}
