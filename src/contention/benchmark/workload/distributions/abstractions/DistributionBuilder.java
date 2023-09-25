package contention.benchmark.workload.distributions.abstractions;

public interface DistributionBuilder {
    Distribution build(int range);

    default StringBuilder toStringBuilder() {
        return toStringBuilder(1);
    }

    StringBuilder toStringBuilder(int indents);

}
