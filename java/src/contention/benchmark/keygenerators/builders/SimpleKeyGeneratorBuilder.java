package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.SimpleKeyGenerator;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SimpleParameters;

public class SimpleKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public SimpleKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        SimpleParameters parameters = (SimpleParameters) this.parameters;
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        SimpleKeyGenerator.data = switch (parameters.distributionBuilder.distributionType) {
            case ZIPF, SKEWED_SETS -> new SimpleKeyGeneratorData(parameters);
            default -> null;
        };

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new SimpleKeyGenerator(
                    parameters.distributionBuilder.getDistribution(parameters.range)
            );
        }

        return keygens;
    }
}
