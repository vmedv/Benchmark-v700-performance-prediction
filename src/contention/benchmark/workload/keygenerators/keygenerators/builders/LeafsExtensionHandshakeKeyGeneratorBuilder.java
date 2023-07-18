package contention.benchmark.workload.keygenerators.keygenerators.builders;

import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilderOld;

public class LeafsExtensionHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilderOld {

    public LeafsExtensionHandshakeKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

//    @Override
//    public void initDataMaps() {
//        LeafsHandshakeParameters parameters = (LeafsHandshakeParameters) this.parameters;
//
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
//
//    }

    @Override
    public KeyGenerator build() {
        return null;
//        return new LeafsExtensionHandshakeKeyGenerator(
////                dataMaps.get("readData"),
////                dataMaps.get("eraseData"),
//                (LeafsHandshakeParameters) this.parameters,
//                range
////                ,
////                parameters.readDistBuilder.getDistribution(),
////                parameters.insertDistBuilder.getDistribution(),
////                parameters.removeDistBuilder.getDistribution()
//        );
    }
}
