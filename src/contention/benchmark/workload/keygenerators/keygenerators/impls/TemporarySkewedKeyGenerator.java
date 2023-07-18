package contention.benchmark.workload.keygenerators.keygenerators.impls;

import contention.benchmark.workload.keygenerators.keygenerators.parameters.TemporarySkewedParameters;
import contention.benchmark.workload.datamap.abstractions.DataMap;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.SkewedUniformDistribution;
import contention.benchmark.workload.distributions.UniformDistribution;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;

public class TemporarySkewedKeyGenerator implements KeyGenerator {
    private final DataMap data;
    private final TemporarySkewedParameters parameters;

    private Distribution[] hotDists;
    private Distribution relaxDist;
    private long time;
    private int pointer;
    private boolean relaxTime;
    private final int range;

    public TemporarySkewedKeyGenerator(DataMap data, TemporarySkewedParameters parameters, int range,
                                       Distribution[] hotDists, Distribution relaxDist) {
        this.data = data;
        this.parameters = parameters;
        this.hotDists = hotDists;
        this.relaxDist = relaxDist;
        this.time = 0;
        this.pointer = 0;
        this.relaxTime = false;
        this.range = range;
    }

    public TemporarySkewedKeyGenerator(DataMap data, TemporarySkewedParameters parameters, int range) {
        this.data = data;
        this.parameters = parameters;
        this.time = 0;
        this.pointer = 0;
        this.relaxTime = false;
        this.range = range;

        this.hotDists = new Distribution[parameters.setCount];

        for (int i = 0; i < parameters.setCount; ++i) {
            this.hotDists[i] = new SkewedUniformDistribution(
                    parameters.setsLengths[i],
                    parameters.hotProbs[i],
                    new UniformDistribution(parameters.setsLengths[i]),
                    new UniformDistribution(range - parameters.setsLengths[i])
            );
        }

        this.relaxDist = new UniformDistribution(range);
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
            if (index >= range) {
                index -= range;
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
