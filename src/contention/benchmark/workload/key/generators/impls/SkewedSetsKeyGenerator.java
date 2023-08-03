package contention.benchmark.workload.key.generators.impls;

import contention.benchmark.workload.distributions.builders.SkewedUniformDistributionBuilder;
import contention.benchmark.workload.key.generators.parameters.SkewedSetsParameters;
import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;

public class SkewedSetsKeyGenerator implements KeyGenerator {
    private  DataMap data;
    private final SkewedSetsParameters parameters;
    private  Distribution readDistribution;
    private  Distribution writeDistribution;

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
        this.readDistribution = new SkewedUniformDistributionBuilder().build(range);
//                .setHotProb(parameters.READ.HOT_PROB).setHotSize(parameters.)
//                        .setParameters(parameters.READ).build(range);
//        this.writeDistribution = new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
//                .setParameters(parameters.WRITE).build(range);
    }

    public SkewedSetsKeyGenerator(SkewedSetsParameters parameters, int range) {
//        this.data = parameters.dataMapBuilder.build(range);
        this.parameters = parameters;
//        this.readDistribution = new DistributionBuilderOld(DistributionType.SKEWED_UNIFORM)
//                .setParameters(parameters.READ).build(range);
//        this.writeDistribution = new DistributionBuilderOld(DistributionType.SKEWED_UNIFORM)
//                .setParameters(parameters.WRITE).build(range);
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

}
