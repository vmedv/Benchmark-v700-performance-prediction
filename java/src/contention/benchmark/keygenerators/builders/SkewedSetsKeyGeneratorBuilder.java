package contention.benchmark.keygenerators.builders;

import contention.abstractions.*;
import contention.benchmark.keygenerators.SkewedSetsKeyGenerator;
import contention.benchmark.keygenerators.data.SkewedSetsKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public SkewedSetsKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        SkewedSetsParameters parameters = (SkewedSetsParameters) this.parameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        SkewedSetsKeyGenerator.data = new SkewedSetsKeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new SkewedSetsKeyGenerator(
                    new DistributionBuilder(DistributionType.SKEWED_SETS)
                            .setParameters(parameters.READ).getDistribution(parameters.range),
                    new DistributionBuilder(DistributionType.SKEWED_SETS)
                            .setParameters(parameters.WRITE).getDistribution(parameters.range)
            );
        }

        return keygens;
    }

}
