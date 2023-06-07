package contention.benchmark.keygenerators.impls;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.distributions.abstractions.Distribution;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;

public class DefaultKeyGenerator implements KeyGenerator {
    private final DataMap data;
    private final Distribution distribution;

    public DefaultKeyGenerator(DataMap data, Distribution distribution) {
        this.data = data;
        this.distribution = distribution;
    }

    private int next() {
        int index = distribution.next();
        return data.get(index);
    }

    @Override
    public int nextGet() {
        return next();
    }

    @Override
    public int nextInsert() {
        return next();
    }

    @Override
    public int nextRemove() {
        return next();
    }

    @Override
    public int nextPrefill() {
        return next();
    }
}
