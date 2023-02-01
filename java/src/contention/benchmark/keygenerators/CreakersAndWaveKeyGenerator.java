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
    public static CreakersAndWaveKeyGeneratorData data;

    public CreakersAndWaveKeyGenerator(Distribution creakersDistribution, MutableDistribution waveDistribution) {
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

}
