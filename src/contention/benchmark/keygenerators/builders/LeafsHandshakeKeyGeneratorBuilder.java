package contention.benchmark.keygenerators.builders;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.datamap.impls.ArrayDataMap;
import contention.benchmark.datamap.impls.IdDataMap;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.LeafsHandshakeKeyGenerator;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

import java.util.List;

public class LeafsHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafsHandshakeKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

//    private DataMap readData;
//    private DataMap eraseData;

//    @Override
//    public void initDataMaps() {
//        LeafsHandshakeParameters parameters = (LeafsHandshakeParameters) this.parameters;
//
//        readData = switch (parameters.readDistBuilder.distributionType) {
//            case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
//            default -> new IdDataMap();
//        };
//        // todo think about the intersection of sets
//        if (parameters.readDistBuilder.distributionType == parameters.removeDistBuilder.distributionType) {
//            eraseData = readData;
//        } else {
//            eraseData = switch (parameters.readDistBuilder.distributionType) {
//                case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
//                default -> new IdDataMap();
//            };
//        }
//    }


    @Override
    public KeyGenerator build() {
        return new LeafsHandshakeKeyGenerator(
//                readData,
//                eraseData,
                (LeafsHandshakeParameters) this.parameters,
                range
//                ,
//                parameters.readDistBuilder.getDistribution(parameters.range),
//                parameters.insertDistBuilder.getDistribution(),
//                parameters.removeDistBuilder.getDistribution(parameters.range)
        );
    }

}

