package contention.benchmark.workload.data.map.builders;

import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.data.map.impls.ArrayDataMap;

import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

public class ArrayDataMapBuilder extends DataMapBuilder {
    transient public int range;

    public ArrayDataMapBuilder setRange(int range) {
        this.range = range;
        return this;
    }

    @Override
    public ArrayDataMapBuilder init(int range) {
        return setRange(range);
    }

    @Override
    public DataMap build() {
        return new ArrayDataMap(range);
    }

    @Override
    public StringBuilder toStringBuilder(int indents) {
        return new StringBuilder(indentedTitleWithData("Type", "ArrayDataMap", indents));
    }
}
