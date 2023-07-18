package contention.benchmark.workload.distributions.builders;

import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.distributions.ZipfDistribution;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.abstractions.MutableDistribution;

public class ZipfianDistributionBuilder implements MutableDistributionBuilder {
    public double alpha = 1.0;
//    public int range = 0;

    public ZipfianDistributionBuilder setAlpha(double alpha) {
        this.alpha = alpha;
        return this;
    }

//    public ZipfianDistributionBuilder setRange(int range) {
//        this.range = range;
//        return this;
//    }

    @Override
    public MutableDistribution build() {
        return new ZipfDistribution(alpha);
    }

    @Override
    public Distribution build(int range) {
        return new ZipfDistribution(alpha, range);
    }

    @Override
    public StringBuilder toStringBuilder() {
        return new StringBuilder()
                .append("\n")
                .append("  alpha:                   \t")
                .append(alpha);
    }
}
