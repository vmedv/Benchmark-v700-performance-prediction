package contention.benchmark.keygenerators.builders;

import contention.benchmark.Parameters;
import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.SkewedSetsKeyGenerator;
import contention.benchmark.datamap.ArrayDataMap;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public SkewedSetsKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

    private DataMap data;
    private Parameters generalParameters;

    @Override
    public void build(Parameters generalParameters) {
        super.build(generalParameters);

        data = new ArrayDataMap(generalParameters);
        this.generalParameters = generalParameters;
    }

    @Override
    public KeyGenerator getKeyGenerator() {
        return new SkewedSetsKeyGenerator(
                data,
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
