package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.MutableDistribution;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.data.CreakersAndWaveKeyGeneratorData;

import java.util.Random;

public class CreakersAndWaveKeyGenerator implements KeyGenerator {
    public static CreakersAndWaveKeyGeneratorData data;
    private Distribution creakersDist;
    private MutableDistribution waveDist;
    private final Random random;

    private int prefillSize;

    public CreakersAndWaveKeyGenerator(Distribution creakersDistribution, MutableDistribution waveDistribution) {
        this.random = new Random();
        this.creakersDist = creakersDistribution;
        this.waveDist = waveDistribution;
        this.prefillSize = 0;
    }

    /**
     *
     * @return true - creakers, false - wave
     */
    private boolean nextCoin() {
        return random.nextDouble() < data.parameters.CREAKERS_PROB;
    }

    @Override
    public int nextRead() {
        int value;
        if (nextCoin()) {
            value = data.get(data.creakersBegin + creakersDist.next());
        } else {
            /**
             * In waveDist the first indexes have a higher probability
             */
            int localWaveBegin = data.waveBegin.get();
            int localWaveEnd = data.waveEnd.get();
            int localWaveLength =
                    (localWaveEnd - localWaveBegin + data.creakersBegin) % data.creakersBegin;
            int index = (localWaveBegin + waveDist.next(localWaveLength)) % data.creakersBegin;
            value = data.get(index);
        }
        return value;
    }

    @Override
    public int nextInsert() {
        int localWaveBegin = data.waveBegin.get();
        int newWaveBegin;
        if (localWaveBegin == 0) {
            newWaveBegin = data.creakersBegin;
        } else {
            newWaveBegin = localWaveBegin - 1;
        }
        data.waveBegin.weakCompareAndSet(localWaveBegin, newWaveBegin);
        return newWaveBegin;
    }

    @Override
    public int nextErase() {
        int localWaveEnd = data.waveEnd.get();
        int newWaveEnd;
        if (localWaveEnd == 0) {
            newWaveEnd = data.creakersBegin;
        } else {
            newWaveEnd = localWaveEnd - 1;
        }
        data.waveEnd.weakCompareAndSet(localWaveEnd, newWaveEnd);
        return newWaveEnd;
    }

    @Override
    public int nextPrefill() {
        return data.get(random.nextInt(Parameters.size));
//        return data.get(data.waveBegin.get() + prefillSize++);
    }

    public int next_warming() {
        return data.get(data.creakersBegin + creakersDist.next());
    }

}
