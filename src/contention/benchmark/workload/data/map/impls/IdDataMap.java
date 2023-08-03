package contention.benchmark.workload.data.map.impls;

import contention.benchmark.workload.data.map.abstractions.DataMap;

public class IdDataMap implements DataMap {
    public int get(int index) {
        return index + 1;
    }
}
