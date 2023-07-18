package contention.benchmark.workload.keygenerators.keygenerators.impls;

import contention.benchmark.workload.keygenerators.keygenerators.parameters.SkewedSetsParameters;
import contention.benchmark.workload.datamap.abstractions.DataMap;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.abstractions.DistributionBuilderOld;
import contention.benchmark.workload.distributions.abstractions.DistributionType;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;

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
        this.readDistribution = new DistributionBuilderOld(DistributionType.SKEWED_UNIFORM)
                        .setParameters(parameters.READ).build(range);
        this.writeDistribution = new DistributionBuilderOld(DistributionType.SKEWED_UNIFORM)
                .setParameters(parameters.WRITE).build(range);
    }

    public SkewedSetsKeyGenerator(SkewedSetsParameters parameters, int range) {
        this.data = parameters.dataMapBuilder.build(range);
        this.parameters = parameters;
        this.readDistribution = new DistributionBuilderOld(DistributionType.SKEWED_UNIFORM)
                .setParameters(parameters.READ).build(range);
        this.writeDistribution = new DistributionBuilderOld(DistributionType.SKEWED_UNIFORM)
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
