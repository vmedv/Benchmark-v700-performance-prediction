package contention.benchmark.workload.keygenerators.keygenerators.builders;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.builders.UniformDistributionBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.datamap.builders.IdDataMapBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.impls.DefaultKeyGenerator;
//import contention.benchmark.keygenerators.parameters.DefaultParameters;

public class DefaultKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public DistributionBuilder distributionBuilder = new UniformDistributionBuilder();
    public DataMapBuilder dataMapBuilder = new IdDataMapBuilder();
    public int range;
//    public DataMap dataMap;

    public DefaultKeyGeneratorBuilder setDistributionBuilder(DistributionBuilder distributionBuilder) {
        this.distributionBuilder = distributionBuilder;
        return this;
    }

    public DefaultKeyGeneratorBuilder setDataMapBuilder(DataMapBuilder dataMapBuilder) {
        this.dataMapBuilder = dataMapBuilder;
        return this;
    }

    @Override
    public KeyGeneratorBuilder init(int range) {
        this.range = range;
        dataMapBuilder.init(range);
//        distributionBuilder
        return this;
    }

    @Override
    public KeyGenerator build() {
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
