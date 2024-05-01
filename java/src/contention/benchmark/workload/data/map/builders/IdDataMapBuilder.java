package contention.benchmark.workload.data.map.builders;

import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.data.map.impls.IdDataMap;

import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

public class IdDataMapBuilder extends DataMapBuilder {
    @Override
    public IdDataMapBuilder init(int range) {
        return this;
    }
    @Override
    public DataMap build() {
        return new IdDataMap();
    }
    @Override
    public StringBuilder toStringBuilder(int indents) {
        return new StringBuilder(indentedTitleWithData("Type", "IdDataMap", indents));
    }
}
