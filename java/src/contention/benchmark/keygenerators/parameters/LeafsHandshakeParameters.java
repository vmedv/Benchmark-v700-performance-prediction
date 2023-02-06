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

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder result = super.toStringBuilder();
        result.append("\n")
                .append("  Key Generator:           \t")
                .append(this.keygenType)
                .append("\n")
                .append("  Read distribution:       \t")
                .append(readDistBuilder.distributionType)
                .append(readDistBuilder.toStringBuilderParameters())
                .append("\n")
                .append("  Insert distribution:     \t")
                .append(insertDistBuilder.distributionType)
                .append(insertDistBuilder.toStringBuilderParameters())
                .append("\n")
                .append("  Erase distribution:      \t")
                .append(eraseDistBuilder.distributionType)
                .append(eraseDistBuilder.toStringBuilderParameters());
        return result;
    }

}
