package contention.benchmark.workload.ThreadLoops.impls;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopAbstract;
import contention.benchmark.stop.condition.StopCondition;

import java.lang.reflect.Method;
import java.util.concurrent.atomic.AtomicInteger;

public class InsertThreadLoop extends ThreadLoopAbstract {
    protected InsertThreadLoop(int myThreadNum, DataStructure<Integer> dataStructure,
                               Method[] methods, StopCondition stopCondition) {
        super(myThreadNum, dataStructure, methods, stopCondition);
    }

    @Override
    public void step() {

    }

    @Override
    public void prefill(AtomicInteger prefillSize) {

    }
}
