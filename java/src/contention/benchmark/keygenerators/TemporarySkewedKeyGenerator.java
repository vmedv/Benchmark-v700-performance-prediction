package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGenerator implements KeyGenerator {
    private final KeyGeneratorData data;
    private final TemporarySkewedParameters parameters;

    private Distribution[] hotDists;
    private Distribution relaxDist;
    private long time;
    private int pointer;
    private boolean relaxTime;

    public TemporarySkewedKeyGenerator(KeyGeneratorData data, TemporarySkewedParameters parameters,
                                       Distribution[] hotDists, Distribution relaxDist) {
        this.data = data;
        this.parameters = parameters;
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
            int index = parameters.setsBegins[pointer] + hotDists[pointer].next();
            if (index >= parameters.range) {
                index -= parameters.range;
            }
            value = data.get(index);
        }

        return value;
    }

    @Override
    public int nextGet() {
        return next();
    }

    @Override
    public int nextInsert() {
        return next();
    }

    @Override
    public int nextRemove() {
        return next();
    }

    @Override
    public int nextPrefill() {
        return data.get(relaxDist.next());
    }
}
