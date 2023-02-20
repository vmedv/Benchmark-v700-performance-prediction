package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.CreakersAndWaveKeyGenerator;
import contention.benchmark.keygenerators.data.CreakersAndWaveKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

public class CreakersAndWaveKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public CreakersAndWaveKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        CreakersAndWaveParameters parameters = (CreakersAndWaveParameters) this.parameters;

        KeyGenerator[] keygens = new KeyGenerator[Parameters.numThreads];

        CreakersAndWaveKeyGenerator.data = new CreakersAndWaveKeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < Parameters.numThreads; threadNum++) {
            keygens[threadNum] = new CreakersAndWaveKeyGenerator(
                    parameters.creakersDistBuilder.getDistribution(CreakersAndWaveKeyGenerator.data.creakersLength),
                    parameters.waveDistBuilder.getDistribution()
            );
        }

        return keygens;
    }
}
