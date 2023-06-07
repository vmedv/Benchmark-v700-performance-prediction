package contention.benchmark.datamap;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.datamap.hash.HashFunction;

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
