package contention.abstractions;

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public abstract class KeyGeneratorData {
    protected final int[] data;

    public KeyGeneratorData(Parameters parameters) {
        List<Integer> dataList = IntStream.range(0, parameters.range).boxed().collect(Collectors.toList());
        Collections.shuffle(dataList);
        data = dataList.stream().mapToInt(Integer::intValue).toArray();
    }

    protected KeyGeneratorData() {
        data = null;
    }

    public int get(int index) {
        return data[index];
    }
}
