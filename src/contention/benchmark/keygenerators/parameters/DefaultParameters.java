package contention.benchmark.keygenerators.parameters;

import contention.benchmark.datamap.abstractions.DataMapBuilder;
import contention.benchmark.datamap.abstractions.DataMapType;
import contention.benchmark.distributions.abstractions.DistributionBuilder;
import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;

public class DefaultParameters implements KeyGeneratorParameters {
    public DistributionBuilder distributionBuilder = new DistributionBuilder();
    public DataMapBuilder dataMapBuilder = new DataMapBuilder(DataMapType.ID);

    @Override
    public void init(Parameters parameters) {

    }

    @Override
    public boolean parseArg(ParseArgument args) {
        return distributionBuilder.parseDistribution(args);
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = new StringBuilder();
        params.append("\n")
                .append("  Distribution:            \t")
                .append(distributionBuilder.type)
                .append(distributionBuilder.toStringBuilderParameters());
        return params;
    }
}
