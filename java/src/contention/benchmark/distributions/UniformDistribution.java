package contention.benchmark.distributions;

import contention.abstractions.MutableDistribution;

import java.util.Random;

public class UniformDistribution implements MutableDistribution {
    private int range;
    private final Random random;

    public UniformDistribution(int range) {
        this.range = range;
        random = new Random();
    }

    public UniformDistribution() {
        random = new Random();
    }

    @Override
    public void setRange(int range) {
        this.range = range;
    }

    @Override
    public int next() {
        return random.nextInt(range);
    }
}
