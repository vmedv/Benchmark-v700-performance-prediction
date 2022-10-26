package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;

public class SimpleKeyGenerator implements KeyGenerator {
    private static SimpleKeyGeneratorData data = null;

    public SimpleKeyGenerator(Distribution distribution) {

    }

    public static void setData(SimpleKeyGeneratorData data) {
        SimpleKeyGenerator.data = data;
    }

    @Override
    public int nextRead() {
        return 0;
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
