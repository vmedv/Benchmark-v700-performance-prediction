package contention.benchmark.workload.key.generators.builders;

import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.key.generators.impls.LeafInsertKeyGenerator;
import contention.benchmark.workload.data.map.impls.LeafInsertDataMap;

public class LeafInsertKeyGeneratorBuilder implements KeyGeneratorBuilder {
    transient public int range;
    transient private LeafInsertDataMap data;


    @Override
    public LeafInsertKeyGeneratorBuilder init(int range) {
        this.range = range;
        data = new LeafInsertDataMap(range);

        return this;
    }

    @Override
    public LeafInsertKeyGenerator build() {
        return new LeafInsertKeyGenerator(data, range);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder();
    }
}
