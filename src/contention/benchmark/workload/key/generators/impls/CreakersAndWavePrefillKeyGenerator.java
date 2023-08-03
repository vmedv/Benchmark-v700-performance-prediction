package contention.benchmark.workload.key.generators.impls;

import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;
import contention.benchmark.workload.key.generators.parameters.CreakersAndWaveParameters;

import java.util.Random;

public class CreakersAndWavePrefillKeyGenerator implements KeyGenerator {
    private final DataMap data;
    private final CreakersAndWaveParameters parameters;
    private final Random random;

    public CreakersAndWavePrefillKeyGenerator(DataMap data, CreakersAndWaveParameters parameters) {
        this.data = data;
        this.parameters = parameters;
        this.random = new Random();
    }

    @Override
    public int nextGet() {
        throw new UnsupportedOperationException();
    }

    @Override
    public int nextInsert() {
        return data.get(parameters.waveBegin.get() + random.nextInt(parameters.prefillSize));
    }

    @Override
    public int nextRemove() {
        throw new UnsupportedOperationException();
    }

}
