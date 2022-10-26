package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;

public class SimpleKeyGenerator implements KeyGenerator {
    public SimpleKeyGenerator(Distribution distribution) {

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
