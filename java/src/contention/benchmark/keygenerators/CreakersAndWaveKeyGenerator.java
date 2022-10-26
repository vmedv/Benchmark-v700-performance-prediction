package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.MutableDistribution;
import contention.benchmark.keygenerators.data.CreakersAndWaveKeyGeneratorData;

public class CreakersAndWaveKeyGenerator implements KeyGenerator {
    private static CreakersAndWaveKeyGeneratorData data;

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
}
