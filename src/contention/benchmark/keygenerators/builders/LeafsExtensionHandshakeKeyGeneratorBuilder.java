package contention.benchmark.keygenerators.builders;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.datamap.ArrayDataMap;
import contention.benchmark.Parameters;
import contention.benchmark.datamap.IdDataMap;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.LeafsExtensionHandshakeKeyGenerator;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

public class LeafsExtensionHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafsExtensionHandshakeKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

    private DataMap readData;
    private DataMap eraseData;
    private Parameters generalParameters;


    @Override
    public void build(Parameters generalParameters) {
        super.build(generalParameters);

        this.generalParameters = generalParameters;
        LeafsHandshakeParameters parameters = (LeafsHandshakeParameters) this.parameters;

         readData = switch (parameters.readDistBuilder.distributionType) {
            case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
            default -> new IdDataMap();
        };
        // todo think about the intersection of sets
        if (parameters.readDistBuilder.distributionType == parameters.removeDistBuilder.distributionType) {
            eraseData = readData;
        } else {
            eraseData = switch (parameters.readDistBuilder.distributionType) {
                case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
                default -> new IdDataMap();
            };
        }
    }

    @Override
    public KeyGenerator getKeyGenerator() {
        return new LeafsExtensionHandshakeKeyGenerator(
                readData,
                eraseData,
                (LeafsHandshakeParameters) this.parameters,
                generalParameters.range
//                ,
//                parameters.readDistBuilder.getDistribution(),
//                parameters.insertDistBuilder.getDistribution(),
//                parameters.removeDistBuilder.getDistribution()
        );
    }
}
