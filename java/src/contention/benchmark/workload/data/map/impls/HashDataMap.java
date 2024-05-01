package contention.benchmark.workload.data.map.impls;

import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.data.map.impls.hash.HashFunction;

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
