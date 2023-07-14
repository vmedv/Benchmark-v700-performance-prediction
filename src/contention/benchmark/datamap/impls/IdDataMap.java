package contention.benchmark.datamap.impls;

import contention.benchmark.datamap.abstractions.DataMap;

public class IdDataMap implements DataMap {
    public int get(int index) {
        return index + 1;
    }
}
