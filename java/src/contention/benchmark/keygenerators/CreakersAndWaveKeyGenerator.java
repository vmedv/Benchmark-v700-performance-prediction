package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.MutableDistribution;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.distributions.ZipfDistribution;
import contention.benchmark.keygenerators.data.CreakersAndWaveKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

public class CreakersAndWaveKeyGenerator implements KeyGenerator {
    private static CreakersAndWaveKeyGeneratorData data;
    private static CreakersAndWaveParameters parameters;

    public CreakersAndWaveKeyGenerator(Distribution creakersDistribution, MutableDistribution waveDistribution) {
    }

    public static void setData(CreakersAndWaveKeyGeneratorData data) {
        CreakersAndWaveKeyGenerator.data = data;
    }

    @Override
    public int nextRead() {
        return 0;//todo
    }

    @Override
    public int nextInsert() {
        return 0;
    }

    @Override
    public int nextErase() {
        return 0;
    }

    @Override
    public int nextPrefill() {
        return 0;
    }

    public static KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        parameters = (CreakersAndWaveParameters) rawParameters;

        KeyGenerator[] keygens = new KeyGenerator[Parameters.numThreads];

        int creakersLength = (int) (Parameters.range * parameters.CREAKERS_SIZE);

        CreakersAndWaveKeyGeneratorData data = new CreakersAndWaveKeyGeneratorData(parameters);

        CreakersAndWaveKeyGenerator.setData(data);

        for (short threadNum = 0; threadNum < Parameters.numThreads; threadNum++) {
            keygens[threadNum] = new CreakersAndWaveKeyGenerator(
                    new UniformDistribution(creakersLength),
                    new ZipfDistribution(parameters.waveZipfParm)
            );
        }

        return keygens;
    }
}
