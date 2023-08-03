package contention.benchmark.workload.thread.loops.builders;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.stop.condition.StopCondition;
import contention.benchmark.workload.key.generators.builders.DefaultKeyGeneratorBuilder;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoop;
import contention.benchmark.workload.thread.loops.impls.DefaultThreadLoop;
import contention.benchmark.workload.thread.loops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;

import java.lang.reflect.Method;

public class DefaultThreadLoopBuilder extends ThreadLoopBuilder {
    public DefaultThreadLoopParameters parameters = new DefaultThreadLoopParameters();

    public KeyGeneratorBuilder keyGeneratorBuilder = new DefaultKeyGeneratorBuilder();


    public DefaultThreadLoopBuilder setWriteRatio(double writeRatio) {
        parameters.insertRatio = writeRatio / 2;
        parameters.removeRatio = writeRatio / 2;
        return this;
    }

    public DefaultThreadLoopBuilder setInsertRatio(double insertRatio) {
        parameters.insertRatio = insertRatio;
        return this;
    }

    public DefaultThreadLoopBuilder setRemoveRatio(double removeRatio) {
        parameters.removeRatio = removeRatio;
        return this;
    }

    public DefaultThreadLoopBuilder setWriteAllsRatio(double writeAllsRatio) {
        parameters.writeAllsRatio = writeAllsRatio;
        return this;
    }

    public DefaultThreadLoopBuilder setSnapshotsRatio(double snapshotsRatio) {
        parameters.snapshotsRatio = snapshotsRatio;
        return this;
    }

    public DefaultThreadLoopBuilder setKeyGeneratorBuilder(KeyGeneratorBuilder keyGeneratorBuilder) {
        this.keyGeneratorBuilder = keyGeneratorBuilder;
        return this;
    }

    @Override
    public DefaultThreadLoopBuilder init(int range) {
        keyGeneratorBuilder.init(range);
        return this;
    }

    @Override
    public DefaultThreadLoop build(int threadNum, DataStructure<Integer> dataStructure,
                            Method[] methods, StopCondition stopCondition) {
        return new DefaultThreadLoop(threadNum, dataStructure, methods, stopCondition,
                keyGeneratorBuilder.build(), parameters);
    }

    @Override
    public StringBuilder toStringBuilder() {
        return parameters.toStringBuilder()
                .append("  KeyGenerator:            \t")
                .append(keyGeneratorBuilder.toStringBuilder());
    }
}
