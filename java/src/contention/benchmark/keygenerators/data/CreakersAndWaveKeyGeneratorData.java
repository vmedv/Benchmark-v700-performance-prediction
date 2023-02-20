package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class CreakersAndWaveKeyGeneratorData extends KeyGeneratorData {
    public final int creakersLength;
    public final int defaultWaveLength;
    public final int creakersBegin;
    public final int prefillSize;

    public final CreakersAndWaveParameters parameters;
    public AtomicInteger waveBegin;
    public AtomicInteger waveEnd;

    public CreakersAndWaveKeyGeneratorData(CreakersAndWaveParameters parameters) {
        super(parameters);
        this.parameters = parameters;
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
