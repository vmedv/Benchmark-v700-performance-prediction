package contention.benchmark.workload.key.generators.impls;

import contention.benchmark.workload.key.generators.parameters.CreakersAndWaveParameters;
import contention.benchmark.workload.data.map.abstractions.DataMap;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.abstractions.MutableDistribution;
import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;

import java.util.Random;

public class CreakersAndWaveKeyGenerator implements KeyGenerator {
    private final DataMap data;
    private final CreakersAndWaveParameters parameters;
    private final Distribution creakersDist;
    private final MutableDistribution waveDist;
    private final Random random;

    public CreakersAndWaveKeyGenerator(DataMap data, CreakersAndWaveParameters parameters, Distribution creakersDistribution, MutableDistribution waveDistribution) {
        this.data = data;
        this.parameters = parameters;
        this.random = new Random();
        this.creakersDist = creakersDistribution;
        this.waveDist = waveDistribution;
    }

    public CreakersAndWaveKeyGenerator(DataMap data, CreakersAndWaveParameters parameters) {
        this.data = data;
        this.parameters = parameters;
        this.random = new Random();
        this.creakersDist = parameters.creakersDistBuilder.build(parameters.creakersLength);
        this.waveDist = parameters.waveDistBuilder.build();
    }

    /**
     *
     * @return true - creakers, false - wave
     */
    private boolean nextCoin() {
        return random.nextDouble() < parameters.creakersProb;
    }

    @Override
    public int nextGet() {
        int value;
        if (nextCoin()) {
            value = data.get(parameters.creakersBegin + creakersDist.next());
        } else {
            /**
             * In waveDist the first indexes have a higher probability
             */
            int localWaveBegin = parameters.waveBegin.get();
            int localWaveEnd = parameters.waveEnd.get();
            int localWaveLength =
                    (localWaveEnd - localWaveBegin + parameters.creakersBegin) % parameters.creakersBegin;
            int index = (localWaveBegin + waveDist.next(localWaveLength)) % parameters.creakersBegin;
            value = data.get(index);
        }
        return value;
    }

    @Override
    public int nextInsert() {
        int localWaveBegin = parameters.waveBegin.get();
        int newWaveBegin;
        if (localWaveBegin == 0) {
            newWaveBegin = parameters.creakersBegin - 1;
        } else {
            newWaveBegin = localWaveBegin - 1;
        }
        parameters.waveBegin.weakCompareAndSet(localWaveBegin, newWaveBegin);
        return data.get(newWaveBegin);
    }

    @Override
    public int nextRemove() {
        int localWaveEnd = parameters.waveEnd.get();
        int newWaveEnd;
        if (localWaveEnd == 0) {
            newWaveEnd = parameters.creakersBegin - 1;
        } else {
            newWaveEnd = localWaveEnd - 1;
        }
        parameters.waveEnd.weakCompareAndSet(localWaveEnd, newWaveEnd);
        return data.get(newWaveEnd);
    }

    public int nextPrefill() {
        return data.get(random.nextInt(parameters.prefillSize));
//        return data.get(data.waveBegin.get() + prefillSize++);
    }

    public int next_warming() {
        return data.get(parameters.creakersBegin + creakersDist.next());
    }

}
