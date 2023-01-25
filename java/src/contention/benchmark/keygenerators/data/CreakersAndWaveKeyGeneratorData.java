package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class CreakersAndWaveKeyGeneratorData extends KeyGeneratorData {
    private final int creakersLength;
    private final int defaultWaveLength;
    private final int creakersBegin;
    private final int prefillSize;

    AtomicInteger waveBegin;
    AtomicInteger waveEnd;

    public CreakersAndWaveKeyGeneratorData(CreakersAndWaveParameters parameters) {
        super(parameters);

        /*
         * data = | ... wave | creakers |
         */
        creakersLength = (int) (Parameters.range * parameters.CREAKERS_SIZE);
        creakersBegin = Parameters.range - creakersLength;
        defaultWaveLength = (int) (Parameters.range * parameters.WAVE_SIZE);
        waveEnd = new AtomicInteger(creakersBegin);
        waveBegin = new AtomicInteger(waveEnd.get() - defaultWaveLength);
        prefillSize = creakersLength + defaultWaveLength;
    }
}
