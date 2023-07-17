package contention.benchmark.keygenerators.parameters;

import contention.benchmark.datamap.abstractions.DataMapBuilder;
import contention.benchmark.datamap.abstractions.DataMapType;
import contention.benchmark.distributions.abstractions.DistributionBuilder;
import contention.benchmark.distributions.abstractions.DistributionType;
import contention.abstractions.ParseArgument;
import contention.benchmark.distributions.parameters.ZipfParameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class LeafsHandshakeParameters implements KeyGeneratorParameters {
    public DistributionBuilder readDistBuilder = new DistributionBuilder();
    public DistributionBuilder insertDistBuilder = new DistributionBuilder()
            .setType(DistributionType.ZIPF).setParameters(new ZipfParameters(1));
    public DistributionBuilder removeDistBuilder = new DistributionBuilder();

    public DataMapBuilder readDataMapBuilder = new DataMapBuilder(DataMapType.ID);
    public DataMapBuilder eraseDataMapBuilder = new DataMapBuilder(DataMapType.ID);

    public AtomicInteger deletedValue;

    // for LeafsExtensionHandshakeKeyGenerator
    public AtomicInteger curRange;

    @Override
    public void init(int range) {
        deletedValue = new AtomicInteger(range/2);
        // for LeafsExtensionHandshakeKeyGenerator
        curRange = new AtomicInteger(10);//Math.max(10, parameters.size));


//        dataMaps.put("readData", switch (parameters.readDistBuilder.distributionType) {
//            case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
//            default -> new IdDataMap();
//        });
//        // todo think about the intersection of sets
//        if (parameters.readDistBuilder.distributionType == parameters.removeDistBuilder.distributionType) {
//            eraseData = readData;
//        } else {
//            eraseData = switch (parameters.readDistBuilder.distributionType) {
//                case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
//                default -> new IdDataMap();
//            };
//        }
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
                .append(readDistBuilder.type)
                .append(readDistBuilder.toStringBuilderParameters())
                .append("\n")
                .append("  Insert distribution:     \t")
                .append(insertDistBuilder.type)
                .append(insertDistBuilder.toStringBuilderParameters())
                .append("\n")
                .append("  Erase distribution:      \t")
                .append(removeDistBuilder.type)
                .append(removeDistBuilder.toStringBuilderParameters());
        return result;
    }

}
