package contention.benchmark.workload.ThreadLoops.builders;

import contention.abstractions.DataStructure;
import contention.benchmark.stop.condition.StopCondition;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoop;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopBuilder;

import java.lang.reflect.Method;

public class TemporaryOperationsThreadLoopBuilder extends ThreadLoopBuilder {
    @Override
    public ThreadLoopBuilder init(int range, StopCondition stopCondition) {
        return null;
    }

    @Override
    public ThreadLoop build(int threadNum, DataStructure<Integer> dataStructure, Method[] methods) {
        return null;
    }

    @Override
    public StringBuilder toStringBuilder() {
        return null;
    }
}
