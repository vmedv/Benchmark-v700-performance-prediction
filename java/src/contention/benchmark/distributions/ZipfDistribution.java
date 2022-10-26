package contention.benchmark.distributions;

import contention.abstractions.MutableDistribution;

import java.util.Random;

public class ZipfDistribution implements MutableDistribution {
    private double area;
    private final double alpha;
    private final Random random;

    public ZipfDistribution(double zipfParm, int range) {
        this(zipfParm);
        setRange(range);
    }

    public ZipfDistribution(double alpha) {
        this.alpha = alpha;
        area = 0;
        random = new Random();
    }

    @Override
    public void setRange(int range) {
        if (alpha == 1.0) {
            area = Math.log(range);
        } else {
            area = Math.pow(range, 1.0 - alpha) / (1.0 - alpha);
        }
    }

    @Override
    public int next() {
        double z = random.nextDouble(); // Uniform random number (0 < z < 1)

        int value;
        double s = area * z;
        if (alpha == 1.0) {
            value = (int) Math.exp(s);
        } else {
            value = (int) Math.pow(s * (1 - alpha), 1 / (1 - alpha));
        }
        return value;
    }
}
