package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.benchmark.Parameters;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class CreakersAndWaveKeyGeneratorData extends KeyGeneratorData {
    private final int creakersLength;
    private final int defaultWaveLength;
    private final int creakersBegin;
    private final int prefillSize;

    AtomicInteger waveBegin;
    AtomicInteger waveEnd;

    public CreakersAndWaveKeyGeneratorData() {
        super();

        /*
         * data = | ... wave | creakers |
         */
        creakersLength = (int) (Parameters.range * CreakersAndWaveParameters.CREAKERS_SIZE);
        creakersBegin = Parameters.range - creakersLength;
        defaultWaveLength = (int) (Parameters.range * CreakersAndWaveParameters.WAVE_SIZE);
        waveEnd = new AtomicInteger(creakersBegin);
        waveBegin = new AtomicInteger(waveEnd.get() - defaultWaveLength);
        prefillSize = creakersLength + defaultWaveLength;
    }
}
