package contention.benchmark.workload;

import contention.benchmark.workload.thread.loops.builders.DefaultThreadLoopBuilder;
import contention.benchmark.workload.stop.condition.OperationCounter;

public class BenchParameters {
    public int iterations = 1;
    public int range = 2048;
    public long afterPrefillDuration = 1000;
    public long afterWarmUpDuration = 0;
    public long betweenIterationsDuration = 100;


    public String benchClassName = "skiplists.lockfree.NonBlockingFriendlySkipListMap";


    public Parameters test;
    public Parameters prefill;
    public Parameters warmUp;

    public BenchParameters() {
        test = new Parameters();
        createDefaultPrefill();
        warmUp = new Parameters();
        // without warmUp
        warmUp.numThreads = 0;
    }

    public BenchParameters(Parameters test, Parameters prefill, Parameters warmUp) {
        this.test = test;
        this.prefill = prefill;
        this.warmUp = warmUp;
    }

    public BenchParameters createDefaultPrefill() {
        prefill = new Parameters();
        prefill.stopCondition = new OperationCounter(range / 2);

        prefill.threadLoopBuilders.add(new DefaultThreadLoopBuilder().setRemoveRatio(0).setInsertRatio(1));

        return this;
    }

    public BenchParameters setIterations(int iterations) {
        this.iterations = iterations;
        return this;
    }

    public BenchParameters setRange(int range) {
        this.range = range;
        return this;
    }

    public BenchParameters setAfterPrefillDuration(long afterPrefillDuration) {
        this.afterPrefillDuration = afterPrefillDuration;
        return this;
    }

    public BenchParameters setAfterWarmUpDuration(long afterWarmUpDuration) {
        this.afterWarmUpDuration = afterWarmUpDuration;
        return this;
    }

    public BenchParameters setBetweenIterationsDuration(long betweenIterationsDuration) {
        this.betweenIterationsDuration = betweenIterationsDuration;
        return this;
    }

    public BenchParameters setBenchClassName(String benchClassName) {
        this.benchClassName = benchClassName;
        return this;
    }

    public BenchParameters setTest(Parameters test) {
        this.test = test;
        return this;
    }

    public BenchParameters setPrefill(Parameters prefill) {
        this.prefill = prefill;
        return this;
    }

    public BenchParameters setWarmUp(Parameters warmUp) {
        this.warmUp = warmUp;
        return this;
    }

    public void init() {
        test.init(range);
        prefill.init(range);
        warmUp.init(range);
    }

}
