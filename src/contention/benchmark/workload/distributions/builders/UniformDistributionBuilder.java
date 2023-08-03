package contention.benchmark.workload.distributions.builders;

import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.distributions.impls.UniformDistribution;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.abstractions.MutableDistribution;

public class UniformDistributionBuilder implements MutableDistributionBuilder {
    @Override
    public UniformDistribution build() {
        return new UniformDistribution();
    }

    @Override
    public UniformDistribution build(int range) {
        return new UniformDistribution(range);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder("Uniform Distribution");
    }
}
