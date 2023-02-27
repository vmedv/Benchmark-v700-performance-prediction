package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.distributions.ZipfDistribution;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SimpleParameters;

public class SimpleKeyGenerator implements KeyGenerator {
    public static SimpleKeyGeneratorData data = null;

    private final Distribution distribution;

    public SimpleKeyGenerator(Distribution distribution) {
        this.distribution = distribution;
    }

    private int next() {
        int value = distribution.next();
        return data.get(value);
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
