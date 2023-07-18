package contention.benchmark.workload.distributions.abstractions;

public interface DistributionBuilder {
    Distribution build(int range);

    StringBuilder toStringBuilder();
}
