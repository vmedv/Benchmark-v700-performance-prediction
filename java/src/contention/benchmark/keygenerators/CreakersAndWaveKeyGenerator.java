package contention.benchmark.keygenerators;

import contention.abstractions.KeyGenerator;

public class CreakersAndWaveKeyGenerator implements KeyGenerator {

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
