package contention.benchmark.workload.ThreadLoops.builders;

import contention.abstractions.DataStructure;
import contention.benchmark.stop.condition.StopCondition;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoop;
import contention.benchmark.workload.ThreadLoops.impls.DefaultThreadLoop;
import contention.benchmark.workload.ThreadLoops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilder;

import java.lang.reflect.Method;

public class DefaultThreadLoopBuilder extends ThreadLoopBuilder {
//    public double numWrites = 0.4;

    public DefaultThreadLoopParameters parameters = new DefaultThreadLoopParameters();
    public StopCondition stopCondition;
//    public double numInsert = 0.2;
//    public double numRemove = 0.2;
//    public double numWriteAlls = 0;
//    public double numSnapshots = 0;
//
//    public KeyGeneratorBuilder keyGeneratorBuilder = new KeyGeneratorBuilder();

    public DefaultThreadLoopBuilder setNumInsert(double numInsert) {
        parameters.numInsert = numInsert;
        return this;
    }

    public DefaultThreadLoopBuilder setNumRemove(double numRemove) {
        parameters.numRemove = numRemove;
        return this;
    }

    public DefaultThreadLoopBuilder setNumWriteAlls(double numWriteAlls) {
        parameters.numWriteAlls = numWriteAlls;
        return this;
    }

    public DefaultThreadLoopBuilder setNumSnapshots(double numSnapshots) {
        parameters.numSnapshots = numSnapshots;
        return this;
    }

    public DefaultThreadLoopBuilder setKeyGeneratorBuilder(KeyGeneratorBuilder keyGeneratorBuilder) {
        parameters.keyGeneratorBuilder = keyGeneratorBuilder;
        return this;
    }

    @Override
    public ThreadLoopBuilder init(int range, StopCondition stopCondition) {
        parameters.init(range);
        return this;
    }

    @Override
    public ThreadLoop build(int threadNum, DataStructure<Integer> dataStructure, Method[] methods) {
        return new DefaultThreadLoop(threadNum, dataStructure, methods, stopCondition, parameters);
    }

    @Override
    public StringBuilder toStringBuilder() {
        return parameters.toStringBuilder();
    }
}
