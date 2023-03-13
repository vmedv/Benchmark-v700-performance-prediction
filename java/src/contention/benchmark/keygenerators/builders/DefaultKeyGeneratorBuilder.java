package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.DefaultKeyGenerator;
import contention.benchmark.keygenerators.parameters.DefaultParameters;

public class DefaultKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public DefaultKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        DefaultParameters parameters = (DefaultParameters) this.parameters;
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        KeyGeneratorData data = switch (parameters.distributionBuilder.distributionType) {
            case ZIPF, SKEWED_UNIFORM -> new KeyGeneratorData(parameters);
            default -> {
                parameters.isNonShuffle = true;
                yield new KeyGeneratorData(parameters);
            }
        };

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new DefaultKeyGenerator(
                    data,
                    parameters.distributionBuilder.getDistribution(parameters.range)
            );
        }

        return keygens;
    }
}
