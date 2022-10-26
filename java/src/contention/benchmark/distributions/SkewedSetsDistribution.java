package contention.benchmark.distributions;

import contention.abstractions.Distribution;

import java.util.Random;

public class SkewedSetsDistribution implements Distribution {
    private final int hotLength;
    private final double hotProb;
    private final Distribution hotDistribution;
    private final Distribution coldDistribution;
    private final Random random;


    public SkewedSetsDistribution(int hotLength, double hotProb,
                                  Distribution hotDistribution,
                                  Distribution coldDistribution) {
        this.hotLength = hotLength;
        this.hotProb = hotProb;
        this.hotDistribution = hotDistribution;
        this.coldDistribution = coldDistribution;
        random = new Random();
    }

    @Override
    public int next() {
        int value;
        if (random.nextDouble() < hotProb) {
            value = hotDistribution.next();
        } else {
            value = hotLength + coldDistribution.next();
        }
        return value;
    }
}
