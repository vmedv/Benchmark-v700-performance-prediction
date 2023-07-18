package contention.benchmark.workload.ThreadLoops.abstractions;

import contention.abstractions.DataStructure;
import contention.benchmark.stop.condition.StopCondition;

import java.lang.reflect.Method;

public abstract class ThreadLoopBuilder {
    public int quantity;

    public ThreadLoopBuilder setQuantity(int quantity) {
        this.quantity = quantity;
        return this;
    }

    public abstract ThreadLoopBuilder init(int range, StopCondition stopCondition);

    public abstract ThreadLoop build(int threadNum, DataStructure<Integer> dataStructure, Method[] methods);

    public abstract StringBuilder toStringBuilder();
}
