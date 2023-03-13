package contention.benchmark.keygenerators.builders;

import contention.abstractions.*;
import contention.benchmark.keygenerators.SkewedSetsKeyGenerator;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public SkewedSetsKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        SkewedSetsParameters parameters = (SkewedSetsParameters) this.parameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        KeyGeneratorData data = new KeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new SkewedSetsKeyGenerator(
                    data,
                    parameters,
                    new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
                            .setParameters(parameters.READ).getDistribution(parameters.range),
                    new DistributionBuilder(DistributionType.SKEWED_UNIFORM)
                            .setParameters(parameters.WRITE).getDistribution(parameters.range)
            );
        }

        return keygens;
    }

}
