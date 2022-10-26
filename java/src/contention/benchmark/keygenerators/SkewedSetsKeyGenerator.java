package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.data.SkewedSetsKeyGeneratorData;

public class SkewedSetsKeyGenerator implements KeyGenerator {
    private static SkewedSetsKeyGeneratorData data;

    public SkewedSetsKeyGenerator(Distribution readDistribution, Distribution writeDistribution) {

    }

    public static void setData(SkewedSetsKeyGeneratorData data) {
        SkewedSetsKeyGenerator.data = data;
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
