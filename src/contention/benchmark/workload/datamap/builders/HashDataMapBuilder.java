package contention.benchmark.workload.datamap.builders;

import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.datamap.impls.HashDataMap;
import contention.benchmark.workload.datamap.impls.hash.HashFunction;
import contention.benchmark.workload.datamap.abstractions.DataMap;

public class HashDataMapBuilder extends DataMapBuilder {
    public HashFunction hashFunction;

    public HashDataMapBuilder setHashFunction(HashFunction hashFunction) {
        this.hashFunction = hashFunction;
        return this;
    }

    @Override
    public DataMapBuilder init(int range) {
        return this;
    }

    @Override
    public DataMap build() {
        return new HashDataMap(hashFunction);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder("HashDataMap");
    }
}
