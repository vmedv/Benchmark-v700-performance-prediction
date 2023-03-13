package contention.benchmark.keygenerators;

import contention.abstractions.*;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

import java.util.Random;

public class CreakersAndWaveKeyGenerator implements KeyGenerator {
    private final KeyGeneratorData data;
    private final CreakersAndWaveParameters parameters;
    private Distribution creakersDist;
    private MutableDistribution waveDist;
    private final Random random;

    public CreakersAndWaveKeyGenerator(KeyGeneratorData data, CreakersAndWaveParameters parameters, Distribution creakersDistribution, MutableDistribution waveDistribution) {
        this.data = data;
        this.parameters = parameters;
        this.random = new Random();
        this.creakersDist = creakersDistribution;
        this.waveDist = waveDistribution;
    }

    /**
     *
     * @return true - creakers, false - wave
     */
    private boolean nextCoin() {
        return random.nextDouble() < parameters.CREAKERS_PROB;
    }

    @Override
    public int nextRead() {
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
    public int nextErase() {
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

    @Override
    public int nextPrefill() {
        return data.get(random.nextInt(parameters.size));
//        return data.get(data.waveBegin.get() + prefillSize++);
    }

    public int next_warming() {
        return data.get(parameters.creakersBegin + creakersDist.next());
    }

}
