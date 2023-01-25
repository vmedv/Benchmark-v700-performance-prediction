package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.distributions.ZipfDistribution;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.data.TemporarySkewedKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGenerator implements KeyGenerator {
    private static TemporarySkewedKeyGeneratorData keygenData;
    private static TemporarySkewedParameters parameters;
    private Distribution[] hotDists;
    private Distribution relaxDist;
    long time;
    int pointer;
    boolean relaxTime;

    public TemporarySkewedKeyGenerator(Distribution[] hotDists, Distribution relaxDist) {
        this.hotDists = hotDists;
        this.relaxDist = relaxDist;
        this.time = 0;
        this.pointer = 0;
        this.relaxTime = false;
    }

    public static void setData(TemporarySkewedKeyGeneratorData keygenData) {
        TemporarySkewedKeyGenerator.keygenData = keygenData;
    }

    private void update_pointer() {
        if (!relaxTime) {
            if (time >= parameters.hotTimes[pointer]) {
                time = 0;
                relaxTime = true;
            }
        } else {
            if (time >= parameters.relaxTimes[pointer]) {
                time = 0;
                relaxTime = false;
                ++pointer;
                if (pointer >= parameters.setCount) {
                    pointer = 0;
                }
            }
        }
        ++time;
    }

    private int next() {
        update_pointer();
        int value;
        if (relaxTime) {
            value = keygenData.get(relaxDist.next());
        } else {
            int index = keygenData.setsBegins[pointer] + hotDists[pointer].next();
            if (index >= parameters.range) {
                index -= parameters.range;
            }
            value = keygenData.get(index);
        }

        return value;
    }

    @Override
    public int nextRead() {
        return next();
    }

    @Override
    public int nextInsert() {
        return next();
    }

    @Override
    public int nextErase() {
        return next();
    }

    @Override
    public int nextPrefill() {
        return keygenData.get(relaxDist.next());
    }

    public static KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        parameters = (TemporarySkewedParameters) rawParameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        TemporarySkewedKeyGeneratorData data = new TemporarySkewedKeyGeneratorData(parameters);

        TemporarySkewedKeyGenerator.setData(data);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            Distribution[] hotDists = new Distribution[parameters.setCount];

            for (int i = 0; i < parameters.setCount; ++i) {
                hotDists[i] = new SkewedSetsDistribution(
                        data.setsLengths[i],
                        parameters.hotProbs[i],
                        new UniformDistribution(data.setsLengths[i]),
                        new UniformDistribution(parameters.range - data.setsLengths[i])
                );
            }

            keygens[threadNum] = new TemporarySkewedKeyGenerator(
                    hotDists, new UniformDistribution(parameters.range)
            );
        }

        return keygens;
    }
}
