package contention.benchmark.workload.keygenerators.keygenerators.parameters;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.distributions.builders.UniformDistributionBuilder;
import contention.benchmark.workload.distributions.builders.ZipfianDistributionBuilder;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.datamap.builders.IdDataMapBuilder;

import java.util.concurrent.atomic.AtomicInteger;

public class LeafsHandshakeParameters {
    public DistributionBuilder readDistBuilder = new UniformDistributionBuilder();
    public MutableDistributionBuilder insertDistBuilder = new ZipfianDistributionBuilder();
    public DistributionBuilder removeDistBuilder = new UniformDistributionBuilder();

    public DataMapBuilder readDataMapBuilder = new IdDataMapBuilder();
    public DataMapBuilder eraseDataMapBuilder = new IdDataMapBuilder();
    public AtomicInteger deletedValue;

    // for LeafsExtensionHandshakeKeyGenerator
    public AtomicInteger curRange;

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


//    public boolean parseArg(ParseArgument args) {
//        switch (args.getCurrent()) {
//            case "-read-dist" -> readDistBuilder.parseDistribution(args.next());
//            case "-insert-dist" -> insertDistBuilder.parseDistribution(args.next());
//            case "-erase-dist" -> removeDistBuilder.parseDistribution(args.next());
//            default -> {
//                return false;
//            }
//        }
//        return true;
//    }

//    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder()
         .append("\n")
                .append("  Key Generator:           \tLEAFS_HANDSHAKE")
                .append("\n")
                .append("  Read distribution:       \t")
                .append(readDistBuilder.toStringBuilder())
                .append("\n")
                .append("  Insert distribution:     \t")
                .append(insertDistBuilder.toStringBuilder())
                .append("\n")
                .append("  Erase distribution:      \t")
                .append(removeDistBuilder.toStringBuilder());
    }

}
