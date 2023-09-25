package contention.benchmark.workload.thread.loops.impls;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.args.generators.abstractions.ArgsGenerator;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoop;
import contention.benchmark.workload.thread.loops.parameters.RatioThreadLoopParameters;
import contention.benchmark.workload.stop.condition.StopCondition;

import java.lang.reflect.Method;
import java.util.Random;
import java.util.Vector;

public class TemporaryOperationsThreadLoop extends ThreadLoop {
    private final ArgsGenerator argsGenerator;
    private long time;
    private int pointer;
    private final int stagesNumber;
    private final int[] stagesDurations;

    /**
     * The random number
     */
    protected Random rand = new Random();
    private final double[][] cdf;

    public TemporaryOperationsThreadLoop(int threadId, DataStructure<Integer> dataStructure,
                                         Method[] methods, StopCondition stopCondition,
                                         int stagesNumber, int[] stagesDurations,
                                         RatioThreadLoopParameters[] ratios,
                                         ArgsGenerator argsGenerator) {
        super(threadId, dataStructure, methods, stopCondition);
        this.argsGenerator = argsGenerator;
        this.time = 0;
        this.pointer = 0;
        this.stagesNumber = stagesNumber;
        this.stagesDurations = stagesDurations;

        this.cdf = new double[stagesNumber][4];

        for (int i = 0; i < stagesNumber; i++) {
            cdf[i][0] = 0;//parameters.numWriteAlls;
            cdf[i][1] = cdf[i][0] + ratios[i].insertRatio;
            cdf[i][2] = cdf[i][1] + ratios[i].removeRatio;
            cdf[i][3] = cdf[i][2];// + parameters.numSnapshots;
        }
    }

    private void update_pointer() {
        if (time >= stagesDurations[pointer]) {
            time = 0;
            ++pointer;
            if (pointer >= stagesNumber) {
                pointer = 0;
            }
        }
        ++time;
    }

    @Override
    public void step() {
        update_pointer();

        double coin = rand.nextDouble();
        if (coin < cdf[pointer][0]) { // 1. should we run a writeAll operation?
            // todo something very strange

            int key = argsGenerator.nextGet();

            // init a collection
            Vector<Integer> vec = new Vector<Integer>(key);
            vec.add(key / 2); // accepts duplicate

            removeAll(vec);
        } else if (coin < cdf[pointer][1]) { // 2. should we run an insert
            int key = argsGenerator.nextInsert();
            insert(key);
        } else if (coin < cdf[pointer][2]) { // 3. should we run a remove
            int key = argsGenerator.nextRemove();
            remove(key);
        } else if (coin < cdf[pointer][3]) { // 4. should we run a readAll operation?
            size();
        } else { //if (coin < cdf[3]) { // 5. then we should run a readSome operation
            int key = argsGenerator.nextGet();
            get(key);
        }
    }


}
