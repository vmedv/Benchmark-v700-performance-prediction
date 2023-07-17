package contention.benchmark;

import contention.benchmark.ThreadLoops.abstractions.ThreadLoopBuilder;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopType;
import contention.benchmark.ThreadLoops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.stop.condition.OperationCounter;

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
//        prefill = new Parameters();
//        prefill.stopCondition = new OperationCounter(test.range / 2);
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

    public void createDefaultPrefill() {
        prefill = new Parameters();
        prefill.stopCondition = new OperationCounter(range / 2);
        DefaultThreadLoopParameters threadLoopParameters = new DefaultThreadLoopParameters();
        threadLoopParameters.numInsert = 1;
        threadLoopParameters.numRemove = 0;
        prefill.threadLoopBuilders.add(new ThreadLoopBuilder(threadLoopParameters).setType(ThreadLoopType.DEFAULT));
    }



    public void init() {
        test.init(range);
        prefill.init(range);
        warmUp.init(range);
    }

}
