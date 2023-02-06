package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.DistributionType;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.parameters.SkewedSetParameters;

public class SimpleParameters extends Parameters {
    public DistributionBuilder distributionBuilder = new DistributionBuilder();

    @Override
    protected void parseArg() {
        int newArgNumber = distributionBuilder.parseDistribution(args, argNumber);

        if (newArgNumber == argNumber) {
            super.parseArg();
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
