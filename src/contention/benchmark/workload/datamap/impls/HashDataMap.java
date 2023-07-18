package contention.benchmark.workload.datamap.impls;

import contention.benchmark.workload.datamap.abstractions.DataMap;
import contention.benchmark.workload.datamap.impls.hash.HashFunction;

public class HashDataMap implements DataMap {
    HashFunction hashFunction;

    public HashDataMap(HashFunction hashFunction) {
        this.hashFunction = hashFunction;
    }

    @Override
    public int get(int index) {
        return hashFunction.hash(index);
    }
}
