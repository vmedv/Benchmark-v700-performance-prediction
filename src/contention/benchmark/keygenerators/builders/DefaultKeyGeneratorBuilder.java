package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.DefaultKeyGenerator;
import contention.benchmark.keygenerators.parameters.DefaultParameters;

public class DefaultKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public DefaultKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

//    @Override
//    public void initDataMaps() {
//        DefaultParameters parameters = (DefaultParameters) this.parameters;
//
//        dataMaps.put("data", switch (parameters.distributionBuilder.distributionType) {
//            case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
//            default -> new IdDataMap();
//        });
//    }

    @Override
    public KeyGenerator build() {
        DefaultParameters parameters = (DefaultParameters) this.parameters;

        return new DefaultKeyGenerator(
//                dataMaps.get("data"),
//                parameters.da
                parameters.dataMapBuilder.build(generalParameters.range),
                parameters.distributionBuilder.build(generalParameters.range)
        );
    }

}
