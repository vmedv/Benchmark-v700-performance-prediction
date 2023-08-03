package contention.benchmark.workload.key.generators.builders;

import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.key.generators.impls.LeafsExtensionHandshakeKeyGenerator;
import contention.benchmark.workload.key.generators.parameters.LeafsHandshakeParameters;

public class LeafsExtensionHandshakeKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public LeafsHandshakeParameters parameters = new LeafsHandshakeParameters();
    public int range;

    public LeafsExtensionHandshakeKeyGeneratorBuilder setParameters(LeafsHandshakeParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public LeafsExtensionHandshakeKeyGeneratorBuilder setReadDistBuilder(DistributionBuilder readDistBuilder) {
        parameters.readDistBuilder = readDistBuilder;
        return this;
    }

    public LeafsExtensionHandshakeKeyGeneratorBuilder setInsertDistBuilder(MutableDistributionBuilder insertDistBuilder) {
        parameters.insertDistBuilder = insertDistBuilder;
        return this;
    }

    public LeafsExtensionHandshakeKeyGeneratorBuilder setRemoveDistBuilder(DistributionBuilder removeDistBuilder) {
        parameters.removeDistBuilder = removeDistBuilder;
        return this;
    }

    public LeafsExtensionHandshakeKeyGeneratorBuilder setReadDataMapBuilder(DataMapBuilder readDataMapBuilder) {
        parameters.readDataMapBuilder = readDataMapBuilder;
        return this;
    }

    public LeafsExtensionHandshakeKeyGeneratorBuilder setEraseDataMapBuilder(DataMapBuilder eraseDataMapBuilder) {
        parameters.eraseDataMapBuilder = eraseDataMapBuilder;
        return this;
    }

    @Override
    public LeafsExtensionHandshakeKeyGeneratorBuilder init(int range) {
        this.range = range;
        parameters.init(range);
        return this;
    }

    @Override
    public LeafsExtensionHandshakeKeyGenerator build() {
        return new LeafsExtensionHandshakeKeyGenerator(
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
