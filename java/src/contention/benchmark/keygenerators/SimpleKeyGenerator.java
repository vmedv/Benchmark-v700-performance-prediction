package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;

public class SimpleKeyGenerator implements KeyGenerator {
    private static SimpleKeyGeneratorData data = null;

    private final Distribution distribution;

    public SimpleKeyGenerator(Distribution distribution) {
        this.distribution = distribution;
    }

    public static void setData(SimpleKeyGeneratorData data) {
        SimpleKeyGenerator.data = data;
    }

    private int next() {
        int value = distribution.next();
        return data == null ? value : data.get(value);
    }

    @Override
    public int nextRead() {
        return next();
    }

    @Override
    public int nextInsert() {
        return next();
    }

    @Override
    public int nextErase() {
        return next();
    }

    @Override
    public int nextPrefill() {
        return next();
    }
}
