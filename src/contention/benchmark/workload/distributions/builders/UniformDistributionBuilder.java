package contention.benchmark.workload.distributions.builders;

import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.distributions.UniformDistribution;
import contention.benchmark.workload.distributions.abstractions.Distribution;
import contention.benchmark.workload.distributions.abstractions.MutableDistribution;

public class UniformDistributionBuilder implements MutableDistributionBuilder {
//    public int range;
//
//    public UniformDistributionBuilder setRange(int range) {
//        this.range = range;
//
//        return this;
//    }

    @Override
    public MutableDistribution build() {
        return new UniformDistribution();
    }

    @Override
    public Distribution build(int range) {
        return new UniformDistribution(range);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder("Uniform Distribution");
    }
}
