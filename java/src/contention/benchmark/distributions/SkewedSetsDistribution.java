package contention.benchmark.distributions;

import contention.abstractions.Distribution;

public class SkewedSetsDistribution implements Distribution {
    public SkewedSetsDistribution(int hotLength, double hotProb,
                                  Distribution hotDistribution,
                                  Distribution coldDistribution1) {
    }

    @Override
    public long next() {
        return 0;//todo
    }
}
