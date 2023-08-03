package contention.benchmark.workload.thread.loops.builders;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.key.generators.builders.DefaultKeyGeneratorBuilder;
import contention.benchmark.workload.stop.condition.StopCondition;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoop;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.thread.loops.impls.TemporaryOperationsThreadLoop;
import contention.benchmark.workload.thread.loops.parameters.TemporaryOperationsThreadLoopParameters;

import java.lang.reflect.Method;

public class TemporaryOperationsThreadLoopBuilder extends ThreadLoopBuilder {
    public TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();


    public TemporaryOperationsThreadLoopBuilder setTempOperCount(int tempOperCount) {
        parameters.setTempOperCount(tempOperCount);
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setOpTimes(int[] opTimes) {
        parameters.opTimes = opTimes;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setNumInserts(double[] numInserts) {
        parameters.numInserts = numInserts;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setNumErases(double[] numErases) {
        parameters.numErases = numErases;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setKeyGeneratorBuilder(KeyGeneratorBuilder keyGeneratorBuilder) {
        parameters.keyGeneratorBuilder = keyGeneratorBuilder;
        return this;
    }

    @Override
    public TemporaryOperationsThreadLoopBuilder init(int range) {
        parameters.init(range);
        return this;
    }

    @Override
    public TemporaryOperationsThreadLoop build(int threadNum, DataStructure<Integer> dataStructure, Method[] methods, StopCondition stopCondition) {
        return new TemporaryOperationsThreadLoop(threadNum, dataStructure, methods, stopCondition, parameters);
    }

    @Override
    public StringBuilder toStringBuilder() {
        return parameters.toStringBuilder();
    }
}
