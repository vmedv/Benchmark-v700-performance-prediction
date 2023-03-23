package contention.benchmark.distributions.parameters;

import contention.abstractions.DistributionParameters;

public class ZipfParameters implements DistributionParameters {
    public double alpha;

    public ZipfParameters(double alpha) {
        this.alpha = alpha;
    }

    @Override
    public StringBuilder toStringBuilder() {
        return new StringBuilder()
                .append("\n")
                .append("  Zipf Parm:               \t")
                .append(alpha);
    }
}
