package contention.benchmark.workload.key.generators.builders;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.builders.UniformDistributionBuilder;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.key.generators.impls.DefaultKeyGenerator;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.data.map.builders.IdDataMapBuilder;

public class DefaultKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public DistributionBuilder distributionBuilder = new UniformDistributionBuilder();
    public DataMapBuilder dataMapBuilder = new IdDataMapBuilder();
    transient public int range;

    public DefaultKeyGeneratorBuilder setDistributionBuilder(DistributionBuilder distributionBuilder) {
        this.distributionBuilder = distributionBuilder;
        return this;
    }

    public DefaultKeyGeneratorBuilder setDataMapBuilder(DataMapBuilder dataMapBuilder) {
        this.dataMapBuilder = dataMapBuilder;
        return this;
    }

    @Override
    public DefaultKeyGeneratorBuilder init(int range) {
        this.range = range;
        dataMapBuilder.init(range);
        return this;
    }

    @Override
    public DefaultKeyGenerator build() {
        return new DefaultKeyGenerator(
                dataMapBuilder.getOrBuild(),
                distributionBuilder.build(range)
        );
    }

    @Override
    public StringBuilder toStringBuilder() {
        return null;
    }

}
