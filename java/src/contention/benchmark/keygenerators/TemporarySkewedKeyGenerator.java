package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.data.TemporarySkewedKeyGeneratorData;

public class TemporarySkewedKeyGenerator implements KeyGenerator {
    private static TemporarySkewedKeyGeneratorData data;

    public TemporarySkewedKeyGenerator(Distribution[] hotDists, Distribution relaxDistribution) {

    }

    public static void setData(TemporarySkewedKeyGeneratorData data) {
        TemporarySkewedKeyGenerator.data = data;
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
