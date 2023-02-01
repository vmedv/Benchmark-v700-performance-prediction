package contention.benchmark.keygenerators.builders;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.keygenerators.TemporarySkewedKeyGenerator;
import contention.benchmark.keygenerators.data.TemporarySkewedKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorBuilder extends KeyGeneratorBuilder {

    @Override
    public KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        TemporarySkewedParameters parameters = (TemporarySkewedParameters) rawParameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        TemporarySkewedKeyGenerator.data = new TemporarySkewedKeyGeneratorData(parameters);
        TemporarySkewedKeyGenerator.parameters = parameters;

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            Distribution[] hotDists = new Distribution[parameters.setCount];

            for (int i = 0; i < parameters.setCount; ++i) {
                hotDists[i] = new SkewedSetsDistribution(
                        TemporarySkewedKeyGenerator.data.setsLengths[i],
                        parameters.hotProbs[i],
                        new UniformDistribution(TemporarySkewedKeyGenerator.data.setsLengths[i]),
                        new UniformDistribution(parameters.range - TemporarySkewedKeyGenerator.data.setsLengths[i])
                );
            }

            keygens[threadNum] = new TemporarySkewedKeyGenerator(
                    hotDists, new UniformDistribution(parameters.range)
            );
        }

        return keygens;
    }

}
