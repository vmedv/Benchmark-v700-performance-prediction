package contention.benchmark.keygenerators.impls;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.distributions.abstractions.Distribution;
import contention.benchmark.distributions.abstractions.DistributionBuilder;
import contention.benchmark.distributions.abstractions.DistributionType;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGenerator implements KeyGenerator {
    private final DataMap data;
    private final SkewedSetsParameters parameters;
    private final Distribution readDistribution;
    private final Distribution writeDistribution;

    public SkewedSetsKeyGenerator(DataMap data, SkewedSetsParameters parameters,
                                  Distribution readDistribution, Distribution writeDistribution) {
        this.data = data;
        this.parameters = parameters;
        this.readDistribution = readDistribution;
        this.writeDistribution = writeDistribution;
    }

    public SkewedSetsKeyGenerator(DataMap data, SkewedSetsParameters parameters, int range) {
        this.data = data;
        this.parameters = parameters;
        this.readDistribution = new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
                        .setParameters(parameters.READ).build(range);
        this.writeDistribution = new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
                .setParameters(parameters.WRITE).build(range);
    }

    public SkewedSetsKeyGenerator(SkewedSetsParameters parameters, int range) {
        this.data = parameters.dataMapBuilder.build(range);
        this.parameters = parameters;
        this.readDistribution = new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
                .setParameters(parameters.READ).build(range);
        this.writeDistribution = new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
                .setParameters(parameters.WRITE).build(range);
    }


    @Override
    public int nextGet() {
        return 0;
    }

    @Override
    public int nextInsert() {
        return 0;
    }

    @Override
    public int nextRemove() {
        return 0;
    }

    @Override
    public int nextPrefill() {
        return 0;
    }

}
