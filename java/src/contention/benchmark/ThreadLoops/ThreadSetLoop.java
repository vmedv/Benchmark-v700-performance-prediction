package contention.benchmark.ThreadLoops;

import contention.abstractions.*;
import contention.benchmark.ThreadLoops.parameters.DefaultThreadLoopParameters;

import java.lang.reflect.Method;
import java.util.Random;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicInteger;

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

    Random rand = new Random();

    /**
     * The distribution of methods as an array of percentiles
     * <p>
     * 0%        cdf[0]        cdf[2]                     100%
     * |--writeAll--|--writeSome--|--readAll--|--readSome--|
     * |-----------write----------|--readAll--|--readSome--| cdf[1]
     */
    double[] cdf = new double[4];

    public ThreadSetLoop(short myThreadNum, CompositionalIntSet bench, Method[] methods,
                         KeyGenerator keygen, DefaultThreadLoopParameters parameters) {
        super(myThreadNum, methods, keygen);
        this.bench = bench;
        /* initialize the method boundaries */
        assert (parameters.numWrites >= parameters.numWriteAlls);
        cdf[0] = parameters.numWriteAlls;
        cdf[1] = parameters.numInsert;
        cdf[2] = cdf[1] + parameters.numErase;
        cdf[3] = cdf[2] + parameters.numSnapshots;
    }

    public void printDataStructure() {
        System.out.println(bench.toString());
    }

    @Override
    public void run() {

        while (!stop) {
            double coin = rand.nextDouble();
            if (coin < cdf[0]) { // 1. should we run a writeAll operation?
                int newInt = keygen.nextRead(); // todo nextWriteAll

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
                int newInt = keygen.nextInsert();

                if (bench.addInt(newInt)) {
                    numAdd++;
                } else {
                    failures++;
                }
            } else if (coin < cdf[2]) { // 3. should we run a remove
                int newInt = keygen.nextErase();

                if (bench.removeInt(newInt)) {
                    numRemove++;
                } else {
                    failures++;
                }

            } else if (coin < cdf[3]) { // 4. should we run a readAll operation?

                bench.size();
                numSize++;

            } else { // 5. then we should run a readSome operation
                int newInt = keygen.nextRead();

                if (bench.containsInt(newInt))
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
    public void prefill(AtomicInteger prefillSize) {
        while (prefillSize.get() > 0) {
            int curPrefillSize = prefillSize.decrementAndGet();
            int v = keygen.nextPrefill();
            if (curPrefillSize < 0 || !bench.addInt(v)) {
                prefillSize.incrementAndGet();
            }
        }
    }
}
