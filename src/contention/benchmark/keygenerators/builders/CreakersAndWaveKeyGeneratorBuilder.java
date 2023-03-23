package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.CreakersAndWaveKeyGenerator;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

public class CreakersAndWaveKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public CreakersAndWaveKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        CreakersAndWaveParameters parameters = (CreakersAndWaveParameters) this.parameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        KeyGeneratorData data = new KeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new CreakersAndWaveKeyGenerator(
                    data,
                    parameters,
                    parameters.creakersDistBuilder.getDistribution(parameters.creakersLength),
                    parameters.waveDistBuilder.getDistribution()
            );
        }

        return keygens;
    }
}
