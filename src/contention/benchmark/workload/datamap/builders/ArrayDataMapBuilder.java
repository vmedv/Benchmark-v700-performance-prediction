package contention.benchmark.workload.datamap.builders;

import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.datamap.impls.ArrayDataMap;
import contention.benchmark.workload.datamap.abstractions.DataMap;

public class ArrayDataMapBuilder extends DataMapBuilder {
    public int range;

    public ArrayDataMapBuilder setRange(int range) {
        this.range = range;
        return this;
    }

    @Override
    public DataMapBuilder init(int range) {
        return setRange(range);
    }

    @Override
    public DataMap build() {
        return new ArrayDataMap(range);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder("ArrayDataMap");
    }
}
