package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.DistributionType;
import contention.abstractions.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.distributions.parameters.ZipfParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class LeafsHandshakeParameters extends Parameters {
    public DistributionBuilder readDistBuilder = new DistributionBuilder();
    public DistributionBuilder insertDistBuilder = new DistributionBuilder()
            .setDistributionType(DistributionType.ZIPF).setParameters(new ZipfParameters(1));
    public DistributionBuilder eraseDistBuilder = new DistributionBuilder();

    public AtomicInteger deletedValue;

    // for LeafsExtensionHandshakeKeyGenerator
    public AtomicInteger curRange;

    public void build() {
        super.build();
        deletedValue = new AtomicInteger(range/2);
        // for LeafsExtensionHandshakeKeyGenerator
        curRange = new AtomicInteger(Math.max(10, size));
    }

    @Override
    protected void parseArg(ParseArgument args) {
        switch (args.getCurrent()) {
            case "-read-dist" -> readDistBuilder.parseDistribution(args.next());
            case "-insert-dist" -> insertDistBuilder.parseDistribution(args.next());
            case "-erase-dist" -> eraseDistBuilder.parseDistribution(args.next());
            default -> super.parseArg(args);
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder result = super.toStringBuilder();
        result.append("\n")
//                .append("  Key Generator:           \t")
//                .append(this.keygenType)
//                .append("\n")
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
