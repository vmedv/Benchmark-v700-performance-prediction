package contention.benchmark.keygenerators;

import contention.abstractions.*;
import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.keygenerators.data.SkewedSetsKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGenerator implements KeyGenerator {
    public static SkewedSetsKeyGeneratorData data;

    private final Distribution readDistribution;
    private final Distribution writeDistribution;

    public SkewedSetsKeyGenerator(Distribution readDistribution, Distribution writeDistribution) {
        this.readDistribution = readDistribution;
        this.writeDistribution = writeDistribution;
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
