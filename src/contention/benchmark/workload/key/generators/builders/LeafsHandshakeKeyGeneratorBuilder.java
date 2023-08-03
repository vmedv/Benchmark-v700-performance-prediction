package contention.benchmark.workload.key.generators.builders;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.key.generators.impls.LeafsHandshakeKeyGenerator;
import contention.benchmark.workload.key.generators.parameters.LeafsHandshakeParameters;

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
    public LeafsHandshakeKeyGeneratorBuilder init(int range) {
        this.range = range;
        parameters.init(range);
        return this;
    }

    @Override
    public LeafsHandshakeKeyGenerator build() {
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

