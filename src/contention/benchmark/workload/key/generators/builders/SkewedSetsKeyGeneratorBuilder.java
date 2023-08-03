package contention.benchmark.workload.key.generators.builders;

import contention.benchmark.workload.distributions.builders.SkewedUniformDistributionBuilder;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.data.map.builders.ArrayDataMapBuilder;
import contention.benchmark.workload.key.generators.impls.SkewedSetsKeyGenerator;

public class SkewedSetsKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public SkewedUniformDistributionBuilder readDistBuilder = new SkewedUniformDistributionBuilder();
    public SkewedUniformDistributionBuilder writeDistBuilder = new SkewedUniformDistributionBuilder();
//    public SkewedUniformParameters READ = new SkewedUniformParameters(0, 0);
//    public SkewedUniformParameters WRITE = new SkewedUniformParameters(0, 0);

    public DataMapBuilder dataMapBuilder = new ArrayDataMapBuilder();

    public double intersection = 0;

    public int readHotLength;
    public int writeHotLength;

    public int writeHotBegin;
    public int writeHotEnd;


    @Override
    public SkewedSetsKeyGeneratorBuilder init(int range) {
        int readHotLength = (int) (range * readDistBuilder.hotSize);
        int writeHotLength = (int) (range * writeDistBuilder.hotSize);
        int intersectionLength = (int) (range * intersection);

        this.readHotLength = readHotLength;
        this.writeHotLength = writeHotLength;

        writeHotBegin = readHotLength - intersectionLength;
        writeHotEnd = writeHotBegin + writeHotLength;

        return this;
    }

    @Override
    public SkewedSetsKeyGenerator build() {
        //TODO build
        return null;
//        return new SkewedSetsKeyGenerator(dataMapBuilder.getOrBuild(), );

//        return new SkewedSetsKeyGenerator(
//                dataMaps.get("data"),
//                (SkewedSetsParameters) this.parameters,
//                range
//                ,
//                new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
//                        .setParameters(parameters.READ).getDistribution(parameters.range),
//                new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
//                        .setParameters(parameters.WRITE).getDistribution(parameters.range)
//        );
    }

    @Override
    public StringBuilder toStringBuilder() {
        return null;
    }

}
