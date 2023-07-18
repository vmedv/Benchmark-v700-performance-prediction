package contention.benchmark.workload.keygenerators.keygenerators.builders;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.impls.LeafsHandshakeKeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.parameters.LeafsHandshakeParameters;

public class LeafsHandshakeKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public LeafsHandshakeParameters parameters = new LeafsHandshakeParameters();
    public int range;

    public LeafsHandshakeKeyGeneratorBuilder setParameters(LeafsHandshakeParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public LeafsHandshakeKeyGeneratorBuilder setReadDistBuilder(DistributionBuilder readDistBuilder) {
        parameters.readDistBuilder = readDistBuilder;
        return this;
    }

    public LeafsHandshakeKeyGeneratorBuilder setInsertDistBuilder(MutableDistributionBuilder insertDistBuilder) {
        parameters.insertDistBuilder = insertDistBuilder;
        return this;
    }

    public LeafsHandshakeKeyGeneratorBuilder setRemoveDistBuilder(DistributionBuilder removeDistBuilder) {
        parameters.removeDistBuilder = removeDistBuilder;
        return this;
    }

    public LeafsHandshakeKeyGeneratorBuilder setReadDataMapBuilder(DataMapBuilder readDataMapBuilder) {
        parameters.readDataMapBuilder = readDataMapBuilder;
        return this;
    }

    public LeafsHandshakeKeyGeneratorBuilder setEraseDataMapBuilder(DataMapBuilder eraseDataMapBuilder) {
        parameters.eraseDataMapBuilder = eraseDataMapBuilder;
        return this;
    }

    @Override
    public KeyGeneratorBuilder init(int range) {
        this.range = range;
        parameters.init(range);
        return this;
    }

    @Override
    public KeyGenerator build() {
        return new LeafsHandshakeKeyGenerator(
                parameters.readDataMapBuilder.getOrBuild(),
                parameters.eraseDataMapBuilder.getOrBuild(),
                this.parameters,
                range
//                ,
//                parameters.readDistBuilder.getDistribution(parameters.range),
//                parameters.insertDistBuilder.getDistribution(),
//                parameters.removeDistBuilder.getDistribution(parameters.range)
        );
    }

    @Override
    public StringBuilder toStringBuilder() {
        return parameters.toStringBuilder();
    }

}

