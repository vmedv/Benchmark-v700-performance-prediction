package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.CreakersAndWaveKeyGenerator;
import contention.benchmark.keygenerators.data.CreakersAndWaveKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

public class CreakersAndWaveKeyGeneratorBuilder extends KeyGeneratorBuilder {

    @Override
    public KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        CreakersAndWaveParameters parameters = (CreakersAndWaveParameters) rawParameters;

        KeyGenerator[] keygens = new KeyGenerator[Parameters.numThreads];

        CreakersAndWaveKeyGenerator.data = new CreakersAndWaveKeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < Parameters.numThreads; threadNum++) {
            keygens[threadNum] = new CreakersAndWaveKeyGenerator(
                    parameters.creakersDist.getDistribution(parameters.range),
                    parameters.waveDist.getDistribution()
            );
        }

        return keygens;
    }
}
