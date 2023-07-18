package contention.benchmark.workload.datamap.builders;

import contention.benchmark.workload.datamap.abstractions.DataMap;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.datamap.impls.IdDataMap;

public class IdDataMapBuilder extends DataMapBuilder {
    @Override
    public DataMapBuilder init(int range) {
        return this;
    }
    @Override
    public DataMap build() {
        return new IdDataMap();
    }
    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder("IdDataMap");
    }
}
