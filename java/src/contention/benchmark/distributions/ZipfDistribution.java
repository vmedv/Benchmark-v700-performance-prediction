package contention.benchmark.distributions;

import contention.abstractions.MutableDistribution;

public class ZipfDistribution implements MutableDistribution {
    public ZipfDistribution(double zipfParm, int range) {
    }

    public ZipfDistribution(double zipfParm) {
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
