package contention.benchmark.workload.distributions.builders;

import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.distributions.impls.ZipfDistribution;

import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

public class ZipfianDistributionBuilder implements MutableDistributionBuilder {
    public double alpha = 1.0;

    public ZipfianDistributionBuilder setAlpha(double alpha) {
        this.alpha = alpha;
        return this;
    }

    @Override
    public ZipfDistribution build() {
        return new ZipfDistribution(alpha);
    }

    @Override
    public ZipfDistribution build(int range) {
        return new ZipfDistribution(alpha, range);
    }

    @Override
    public StringBuilder toStringBuilder(int indents) {
        return new StringBuilder()
                .append(indentedTitleWithData("Type", "Zipfian", indents))
                .append(indentedTitleWithData("alpha", alpha, indents));
    }
}
