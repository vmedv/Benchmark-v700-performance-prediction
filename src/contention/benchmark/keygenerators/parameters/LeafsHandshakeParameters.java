package contention.benchmark.keygenerators.parameters;

import contention.benchmark.distributions.abstractions.DistributionBuilder;
import contention.benchmark.distributions.abstractions.DistributionType;
import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.distributions.parameters.ZipfParameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class LeafsHandshakeParameters implements KeyGeneratorParameters {
    public DistributionBuilder readDistBuilder = new DistributionBuilder();
    public DistributionBuilder insertDistBuilder = new DistributionBuilder()
            .setDistributionType(DistributionType.ZIPF).setParameters(new ZipfParameters(1));
    public DistributionBuilder removeDistBuilder = new DistributionBuilder();

    public AtomicInteger deletedValue;

    // for LeafsExtensionHandshakeKeyGenerator
    public AtomicInteger curRange;

    @Override
    public void build(Parameters parameters) {
        deletedValue = new AtomicInteger(parameters.range/2);
        // for LeafsExtensionHandshakeKeyGenerator
        curRange = new AtomicInteger(Math.max(10, parameters.size));

    }

    @Override
    public boolean parseArg(ParseArgument args) {
        switch (args.getCurrent()) {
            case "-read-dist" -> readDistBuilder.parseDistribution(args.next());
            case "-insert-dist" -> insertDistBuilder.parseDistribution(args.next());
            case "-erase-dist" -> removeDistBuilder.parseDistribution(args.next());
            default -> {
                return false;
            }
        }
        return true;
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder result = new StringBuilder();
        result.append("\n")
                .append("  Key Generator:           \tLEAFS_HANDSHAKE")
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
                .append(removeDistBuilder.distributionType)
                .append(removeDistBuilder.toStringBuilderParameters());
        return result;
    }

}
