package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.Parameters;

public class SimpleParameters extends Parameters {
    public DistributionBuilder distributionBuilder = new DistributionBuilder();

    @Override
    protected void parseArg() {
        int newArgNumber = distributionBuilder.parseDistribution(args, argNumber);

        if (newArgNumber == argNumber) {
            super.parseArg();
        } else {
            argNumber = newArgNumber;
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
