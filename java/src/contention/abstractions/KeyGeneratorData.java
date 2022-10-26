package contention.abstractions;

import contention.benchmark.Parameters;

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public abstract class KeyGeneratorData {
    protected final int[] data;

    public KeyGeneratorData() {
        List<Integer> dataList = IntStream.range(0, Parameters.range).boxed().collect(Collectors.toList());
        Collections.shuffle(dataList);
        data = dataList.stream().mapToInt(Integer::intValue).toArray();
    }

    public int get(int index) {
        return data[index];
    }
}
