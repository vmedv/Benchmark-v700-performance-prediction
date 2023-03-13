package contention.benchmark.keygenerators.data;

import contention.abstractions.Parameters;

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class KeyGeneratorData {
    protected final int[] data;
    private final boolean isNonShuffle;
    public KeyGeneratorData(Parameters parameters) {
        isNonShuffle = parameters.isNonShuffle;
        if (!parameters.isNonShuffle) {
            List<Integer> dataList = IntStream.range(0, parameters.range).boxed().collect(Collectors.toList());
            Collections.shuffle(dataList);
            data = dataList.stream().mapToInt(Integer::intValue).toArray();
        } else {
            data = null;
        }
    }

    protected KeyGeneratorData() {
        data = null;
        isNonShuffle = true;
    }

    public int get(int index) {
        return !isNonShuffle ? data[index] : index + 1;
    }
}
