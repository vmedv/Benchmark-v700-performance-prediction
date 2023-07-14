package contention.benchmark.datamap.impls;

import contention.benchmark.Parameters;
import contention.benchmark.datamap.abstractions.DataMap;

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class ArrayDataMap implements DataMap {
    protected final int[] data;

    public ArrayDataMap(Parameters parameters) {
        List<Integer> dataList = IntStream.range(0, parameters.range).boxed().collect(Collectors.toList());
        Collections.shuffle(dataList);
        data = dataList.stream().mapToInt(Integer::intValue).toArray();
    }

    public ArrayDataMap(int range) {
        List<Integer> dataList = IntStream.range(0, range).boxed().collect(Collectors.toList());
        Collections.shuffle(dataList);
        data = dataList.stream().mapToInt(Integer::intValue).toArray();
    }

    public int get(int index) {
        return data[index];
    }
}
