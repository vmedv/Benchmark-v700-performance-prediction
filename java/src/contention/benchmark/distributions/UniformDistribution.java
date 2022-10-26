package contention.benchmark.distributions;

import contention.abstractions.MutableDistribution;

public class UniformDistribution implements MutableDistribution {
    public UniformDistribution(int range) {

    }

    @Override
    public long next() {
        return 0;//todo
    }

    @Override
    public void setMaxKey(int maxKey) {

    }

    @Override
    public int next(int maxKey) {
        return 0;
    }
}
