package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.datamap.impls.ArrayDataMap;
import contention.benchmark.datamap.impls.IdDataMap;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.LeafsExtensionHandshakeKeyGenerator;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

public class LeafsExtensionHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

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
        return new LeafsExtensionHandshakeKeyGenerator(
//                dataMaps.get("readData"),
//                dataMaps.get("eraseData"),
                (LeafsHandshakeParameters) this.parameters,
                generalParameters.range
//                ,
//                parameters.readDistBuilder.getDistribution(),
//                parameters.insertDistBuilder.getDistribution(),
//                parameters.removeDistBuilder.getDistribution()
        );
    }
}
