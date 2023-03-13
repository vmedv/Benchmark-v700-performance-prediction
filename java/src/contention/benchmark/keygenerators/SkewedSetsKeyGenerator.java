package contention.benchmark.keygenerators;

import contention.abstractions.*;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGenerator implements KeyGenerator {
    private final KeyGeneratorData data;
private final SkewedSetsParameters parameters;
    private final Distribution readDistribution;
    private final Distribution writeDistribution;

    public SkewedSetsKeyGenerator(KeyGeneratorData data, SkewedSetsParameters parameters,Distribution readDistribution, Distribution writeDistribution) {
        this.data = data;
        this.parameters = parameters;
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
