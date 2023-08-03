package contention.benchmark.workload.thread.loops.impls;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoop;
import contention.benchmark.workload.thread.loops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;
import contention.benchmark.workload.stop.condition.StopCondition;

import java.lang.reflect.Method;
import java.util.Random;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicInteger;

public class DefaultThreadLoop extends ThreadLoop {
    private final KeyGenerator keygen;
    protected Random rand = new Random();

    /**
     * The distribution of methods as an array of percentiles
     * <p>
     * 0%        cdf[0]        cdf[2]                     100%
     * |--writeAll--|--writeSome--|--readAll--|--readSome--|
     * |-----------write----------|--readAll--|--readSome--| cdf[1]
     */
    private double[] cdf = new double[4];

    public DefaultThreadLoop(int threadNum, DataStructure<Integer> dataStructure,
                             Method[] methods, StopCondition stopCondition,
                         KeyGenerator keygen, DefaultThreadLoopParameters parameters) {
        super(threadNum, dataStructure, methods, stopCondition);
        /* initialize the method boundaries */
        this.keygen = keygen;
//        assert (parameters.numWrites >= parameters.writeAllsRatio);
        cdf[0] = parameters.writeAllsRatio;
        cdf[1] = parameters.insertRatio;
        cdf[2] = cdf[1] + parameters.removeRatio;
        cdf[3] = cdf[2] + parameters.snapshotsRatio;
    }

    @Override
    public void step() {
        double coin = rand.nextDouble();
        if (coin < cdf[0]) { // 1. should we run a writeAll operation?
            // todo something very strange

            int key = keygen.nextGet();

            // init a collection
            Vector<Integer> vec = new Vector<Integer>(key);
            vec.add(key / 2); // accepts duplicate

            removeAll(vec);
        } else if (coin < cdf[1]) { // 2. should we run an insert
            int key = keygen.nextInsert();
            insert(key);
        } else if (coin < cdf[2]) { // 3. should we run a remove
            int key = keygen.nextRemove();
            remove(key);
        } else if (coin < cdf[3]) { // 4. should we run a readAll operation?
            size();
        } else { //if (coin < cdf[3]) { // 5. then we should run a readSome operation
            int key = keygen.nextGet();
            get(key);
        } //else {
//                sleep(Parameters.sleepTime);
        // warmup для определения sleepTime
//            }
    }


//    public void prefill(AtomicInteger prefillSize) {
//        while (prefillSize.get() > 0) {
//            int curPrefillSize = prefillSize.decrementAndGet();
//            int v = keygen.nextPrefill();
//            if (curPrefillSize < 0 || dataStructure.insert(v) != null) {
//                prefillSize.incrementAndGet();
//            }
//        }
//    }
}
