package contention.benchmark.keygenerators.builders;

import contention.abstractions.*;
import contention.benchmark.distributions.SkewedUniformDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.keygenerators.TemporarySkewedKeyGenerator;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public TemporarySkewedKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        TemporarySkewedParameters parameters = (TemporarySkewedParameters) this.parameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        KeyGeneratorData data = new KeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            Distribution[] hotDists = new Distribution[parameters.setCount];

            for (int i = 0; i < parameters.setCount; ++i) {
                hotDists[i] = new SkewedUniformDistribution(
                        parameters.setsLengths[i],
                        parameters.hotProbs[i],
                        new UniformDistribution(parameters.setsLengths[i]),
                        new UniformDistribution(parameters.range - parameters.setsLengths[i])
                );
            }

            keygens[threadNum] = new TemporarySkewedKeyGenerator(
                    data,
                    parameters,
                    hotDists,
                    new UniformDistribution(parameters.range)
            );
        }

        return keygens;
    }

}
