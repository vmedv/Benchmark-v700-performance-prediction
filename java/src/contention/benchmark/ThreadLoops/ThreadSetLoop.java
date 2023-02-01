package contention.benchmark.ThreadLoops;

import contention.abstractions.CompositionalIntSet;
import contention.abstractions.CompositionalMap;
import contention.abstractions.KeyGenerator;
import contention.abstractions.ThreadLoopAbstract;
import contention.abstractions.Parameters;

import java.lang.reflect.Method;
import java.util.Random;
import java.util.Vector;

/**
 * The loop executed by each thread of the integer set
 * benchmark.
 *
 * @author Vincent Gramoli
 */
public class ThreadSetLoop extends ThreadLoopAbstract {

    /**
     * The instance of the running benchmark
     */
    public CompositionalIntSet bench;
    /**
     * The pool of methods that can run
     */
    protected Method[] methods;
    /**
     * The number of the current thread
     */
    protected final short myThreadNum;

    Random rand = new Random();

    /**
     * The distribution of methods as an array of percentiles
     * <p>
     * 0%        cdf[0]        cdf[2]                     100%
     * |--writeAll--|--writeSome--|--readAll--|--readSome--|
     * |-----------write----------|--readAll--|--readSome--| cdf[1]
     */
    int[] cdf = new int[4];

    public ThreadSetLoop(short myThreadNum, CompositionalIntSet bench, Method[] methods,
                         KeyGenerator keygen, Parameters parameters) {
        super(keygen, parameters);
        this.myThreadNum = myThreadNum;
        this.bench = bench;
        this.methods = methods;
        /* initialize the method boundaries */
        assert (parameters.numWrites >= parameters.numWriteAlls);
        cdf[0] = 10 * parameters.numWriteAlls;
        cdf[1] = 10 * parameters.numInsert;
        cdf[2] = cdf[1] + 10 * parameters.numErase;
        cdf[3] = cdf[2] + 10 * parameters.numSnapshots;
    }

    public void printDataStructure() {
        System.out.println(bench.toString());
    }

    @Override
    public void run() {

        while (!stop) {
            Integer newInt = rand.nextInt(parameters.range);
            int coin = rand.nextInt(1000);
            if (coin < cdf[0]) { // 1. should we run a writeAll operation?

                // init a collection
                Vector<Integer> vec = new Vector<Integer>(newInt);
                vec.add(newInt / 2); // accepts duplicate

                try {
                    if (bench.removeAll(vec))
                        numRemoveAll++;
                    else failures++;
                } catch (Exception e) {
                    System.err.println("Unsupported writeAll operations! Leave the default value of the numWriteAlls parameter (0).");
                }

            } else if (coin < cdf[1]) { // 2. should we run an add
                    if (bench.addInt((int) newInt)) {
                        numAdd++;
                    } else {
                        failures++;
                    }
            } else if (coin < cdf[2]) { // 3. should we run a remove
                    if (bench.removeInt((int) newInt)) {
                        numRemove++;
                    } else {
                        failures++;
                    }

            } else if (coin < cdf[3]) { // 4. should we run a readAll operation?

                bench.size();
                numSize++;

            } else { // 5. then we should run a readSome operation

                if (bench.containsInt((int) newInt))
                    numContains++;
                else
                    failures++;
            }
            total++;

            assert total == failures + numContains + numSize + numRemove
                    + numAdd + numRemoveAll + numAddAll;
        }
        this.getCount = CompositionalMap.counts.get().getCount;
        this.nodesTraversed = CompositionalMap.counts.get().nodesTraversed;
        this.structMods = CompositionalMap.counts.get().structMods;
        System.out.println("Thread #" + myThreadNum + " finished.");
    }

    @Override
    public void prefill() {
        long size = parameters.size / parameters.numPrefillThreads;
        for (long i = size; i > 0; ) {
            int v = rand.nextInt(parameters.range);
            if (bench.addInt(v)) {
                i--;
            }
        }
    }
}
