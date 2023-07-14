package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.SkewedSetsKeyGenerator;
import contention.benchmark.datamap.impls.ArrayDataMap;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public SkewedSetsKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

//    @Override
//    public void initDataMaps() {
//        dataMaps.put("data", new ArrayDataMap(generalParameters));
//    }

    @Override
    public KeyGenerator build() {
        return new SkewedSetsKeyGenerator(
//                dataMaps.get("data"),
                (SkewedSetsParameters) this.parameters,
                generalParameters.range
//                ,
//                new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
//                        .setParameters(parameters.READ).getDistribution(parameters.range),
//                new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
//                        .setParameters(parameters.WRITE).getDistribution(parameters.range)
        );
    }

}
