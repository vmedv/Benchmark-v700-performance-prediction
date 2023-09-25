package contention.benchmark.workload.distributions.builders;

import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.distributions.impls.UniformDistribution;

import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

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
    public StringBuilder toStringBuilder(int indents) {
        return new StringBuilder(indentedTitleWithData("Type", "Uniform", indents));
    }
}
