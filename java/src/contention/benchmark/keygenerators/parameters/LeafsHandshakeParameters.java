package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.DistributionType;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.parameters.ZipfParameters;

public class LeafsHandshakeParameters extends Parameters {
    public DistributionBuilder readDistBuilder = new DistributionBuilder();
    public DistributionBuilder insertDistBuilder = new DistributionBuilder()
            .setDistributionType(DistributionType.ZIPF).setParameters(new ZipfParameters(1));
    public DistributionBuilder eraseDistBuilder = new DistributionBuilder();

    @Override
    protected void parseArg() {
        switch (args[argNumber]) {
            case "-read-dist" -> argNumber = readDistBuilder.parseDistribution(args, argNumber);
            case "-insert-dist" -> argNumber = insertDistBuilder.parseDistribution(args, argNumber);
            case "-erase-dist" -> argNumber = eraseDistBuilder.parseDistribution(args, argNumber);
            default -> super.parseArg();
        }
    }

}
