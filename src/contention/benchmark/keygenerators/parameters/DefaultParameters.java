package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.Parameters;
import contention.abstractions.ParseArgument;

public class DefaultParameters extends Parameters {
    public DistributionBuilder distributionBuilder = new DistributionBuilder();

    @Override
    protected void parseArg(ParseArgument args) {
        if (!distributionBuilder.parseDistribution(args)) {
            super.parseArg(args);
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = super.toStringBuilder();
        params.append("\n")
                .append("  Distribution:            \t")
                .append(distributionBuilder.distributionType)
                .append(distributionBuilder.toStringBuilderParameters());
        return params;
    }
}
