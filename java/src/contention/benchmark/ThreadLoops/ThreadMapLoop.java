package contention.benchmark.ThreadLoops;

import contention.abstractions.CompositionalMap;
import contention.abstractions.KeyGenerator;
import contention.abstractions.ThreadLoopAbstract;
import contention.benchmark.Parameters;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

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
     * The number of the current thread
     */
    protected final short myThreadNum;

    /**
     * The random number
     */
    Random rand = new Random();

    /**
     * The distribution of methods as an array of percentiles
     * <p>
     * 0%        cdf[0]        cdf[2]                     100%
     * |--writeAll--|--writeSome--|--readAll--|--readSome--|
     * |-----------write----------|--readAll--|--readSome--| cdf[1]
     */
    int[] cdf = new int[3];

    public ThreadMapLoop(short myThreadNum,
                         CompositionalMap<Integer, Integer> bench, Method[] methods, KeyGenerator keygen) {
        super(keygen);
        this.myThreadNum = myThreadNum;
        this.bench = bench;
        this.methods = methods;
        /* initialize the method boundaries */
        assert (Parameters.numWrites >= Parameters.numWriteAlls);
        cdf[0] = 10 * Parameters.numWriteAlls;
        cdf[1] = 10 * Parameters.numWrites;
        cdf[2] = cdf[1] + 10 * Parameters.numSnapshots;
    }

    public void printDataStructure() {
        System.out.println(bench.toString());
    }

    @Override
    public void run() {

        while (!stop) {
            int newInt = rand.nextInt(Parameters.range);
            Integer a, b;
            int coin = rand.nextInt(1000);
            if (coin < cdf[0]) { // 1. should we run a writeAll operation?

                // init a collection
                Map<Integer, Integer> hm = new HashMap<Integer, Integer>(newInt, newInt);
                hm.put(newInt / 2, newInt / 2); // accepts duplicate

                try {
                    if (bench.keySet().removeAll(hm.keySet()))
                        numRemoveAll++;
                    else failures++;
                } catch (Exception e) {
                    System.err.println("Unsupported writeAll operations! Leave the default value of the numWriteAlls parameter (0).");
                }


            } else if (coin < cdf[1]) { // 2. should we run a writeSome
                // operation?
                if (2 * (coin - cdf[0]) < cdf[1] - cdf[0]) { // add
                    newInt = keygen.nextInsert();

                    if ((a = bench.putIfAbsent(newInt, newInt)) == null) {
                        numAdd++;
                    } else {
                        failures++;
                    }
                } else { // remove
                    newInt = keygen.nextErase();

                    if ((a = bench.remove(newInt)) != null) {
                        numRemove++;
                    } else
                        failures++;
                }
                //
            } else if (coin < cdf[2]) { // 3. should we run a readAll operation?

                bench.size();
                numSize++;

            } else { //if (coin < cdf[3]) { // 4. then we should run a readSome operation

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
    public void prefill() {
        long size = Parameters.range / Parameters.numPrefillThreads;
        for (long i = size; i > 0; ) {
            int v = rand.nextInt(Parameters.range);
            if (bench.putIfAbsent(v, v) == null) {
                i--;
            }
        }
    }

}
