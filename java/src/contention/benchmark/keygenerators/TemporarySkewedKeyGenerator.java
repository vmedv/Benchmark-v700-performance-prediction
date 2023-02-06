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
    public static TemporarySkewedKeyGeneratorData data;
    public static TemporarySkewedParameters parameters;

    private Distribution[] hotDists;
    private Distribution relaxDist;
    private long time;
    private int pointer;
    private boolean relaxTime;

    public TemporarySkewedKeyGenerator(Distribution[] hotDists, Distribution relaxDist) {
        this.hotDists = hotDists;
        this.relaxDist = relaxDist;
        this.time = 0;
        this.pointer = 0;
        this.relaxTime = false;
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
            value = data.get(relaxDist.next());
        } else {
            int index = data.setsBegins[pointer] + hotDists[pointer].next();
            if (index >= parameters.range) {
                index -= parameters.range;
            }
            value = data.get(index);
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
        return data.get(relaxDist.next());
    }
}
