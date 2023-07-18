package contention.benchmark.workload.datamap.impls;

import contention.benchmark.workload.datamap.abstractions.DataMap;

public class IdDataMap implements DataMap {
    public int get(int index) {
        return index + 1;
    }
}
