package contention.benchmark.workload.keygenerators.keygenerators.builders;

import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.impls.TemporarySkewedKeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public TemporarySkewedParameters parameters = new TemporarySkewedParameters();
    public int range;

    public TemporarySkewedKeyGeneratorBuilder setDataMapBuilder(DataMapBuilder dataMapBuilder) {
        parameters.dataMapBuilder = dataMapBuilder;
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setSetCount(final int setCount) {
        parameters.setSetCount(setCount);
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setSetSize(final int i, final double setSize) {
        parameters.setSetSize(i, setSize);
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setHotProb(final int i, final double hotProb) {
        parameters.setHotProb(i, hotProb);
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setHotTime(final int i, final int hotTime) {
        parameters.setHotTime(i, hotTime);
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setRelaxTimes(final int i, final int relaxTime) {
        parameters.setRelaxTimes(i, relaxTime);
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setCommonHotTime(final int hotTime) {
        parameters.setCommonHotTime(hotTime);
        return this;
    }

    public TemporarySkewedKeyGeneratorBuilder setCommonRelaxTime(final int relaxTime) {
        parameters.setCommonRelaxTime(relaxTime);
        return this;
    }

    @Override
    public KeyGeneratorBuilder init(int range) {
        this.range = range;
        parameters.dataMapBuilder.init(range);
        return this;
    }

    @Override
    public KeyGenerator build() {

        return new TemporarySkewedKeyGenerator(
                parameters.dataMapBuilder.getOrBuild(),
                parameters,
                range
        );
    }

    @Override
    public StringBuilder toStringBuilder() {
        return parameters.toStringBuilder();
    }

}
