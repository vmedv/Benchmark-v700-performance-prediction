package contention.benchmark.ThreadLoops.impls;

import contention.abstractions.CompositionalMap;
import contention.abstractions.KeyGenerator;
import contention.abstractions.ThreadLoopAbstract;
import contention.benchmark.ThreadLoops.parameters.TemporaryOperationsThreadLoopParameters;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

public class TemporaryOperationsThreadLoop extends ThreadLoopAbstract {
    /**
     * The instance of the running benchmark
     */
    public final CompositionalMap<Integer, Integer> bench;
    private long time;
    private int pointer;
    private final TemporaryOperationsThreadLoopParameters parameters;

    /**
     * The random number
     */
    protected Random rand = new Random();
    private final double[][] cdf;

    public TemporaryOperationsThreadLoop(short myThreadNum,
                                         CompositionalMap<Integer, Integer> bench,
                                         Method[] methods,
                                         KeyGenerator keygen, TemporaryOperationsThreadLoopParameters parameters) {
        super(myThreadNum, methods, keygen);
        this.bench = bench;
        this.time = 0;
        this.pointer = 0;
        this.parameters = parameters;

        this.cdf = new double[parameters.tempOperCount][4];

        for (int i = 0; i < parameters.tempOperCount; i++) {
            cdf[i][0] = 0;//parameters.numWriteAlls;
            cdf[i][1] = parameters.numInserts[i];
            cdf[i][2] = cdf[i][1] + parameters.numErases[i];
            cdf[i][3] = cdf[i][2];// + parameters.numSnapshots;
        }
    }

    private void update_pointer() {
        if (time >= parameters.opTimes[pointer]) {
            time = 0;
            ++pointer;
            if (pointer >= parameters.tempOperCount) {
                pointer = 0;
            }
        }
        ++time;
    }

    @Override
    public void run() {

        while (!stop) {
            update_pointer();

            Integer a, b;
            double coin = rand.nextDouble();
            if (coin < cdf[pointer][0]) { // 1. should we run a writeAll operation?
                int newInt = keygen.nextRead();

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


            } else if (coin < cdf[pointer][1]) { // 2. should we run an insert

                int newInt = keygen.nextInsert();

                if ((a = bench.putIfAbsent(newInt, newInt)) == null) {
                    numAdd++;
                } else {
                    failures++;
                }
            } else if (coin < cdf[pointer][2]) { // 3. should we run a remove

                int newInt = keygen.nextErase();

                if ((a = bench.remove(newInt)) != null) {
                    numRemove++;
                } else {
                    failures++;
                }

            } else if (coin < cdf[pointer][3]) { // 4. should we run a readAll operation?

                bench.size();
                numSize++;

            } else { //if (coin < cdf[3]) { // 5. then we should run a readSome operation

                int newInt = keygen.nextRead();

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
            if (curPrefillSize < 0 || bench.putIfAbsent(v, v) != null) {
                prefillSize.incrementAndGet();
            }
        }
    }

}
