package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.datamap.impls.ArrayDataMap;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.CreakersAndWaveKeyGenerator;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

public class CreakersAndWaveKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public CreakersAndWaveKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

//    private DataMap data;

//    @Override
//    public void initDataMaps() {
//        dataMaps.put("data", new ArrayDataMap(generalParameters));
//    }

    @Override
    public KeyGenerator build() {
        CreakersAndWaveParameters parameters = (CreakersAndWaveParameters) this.parameters;

        return new CreakersAndWaveKeyGenerator(
//                dataMaps.get("data"),
                parameters.dataMapBuilder.build(generalParameters.range),
                parameters
//                ,
//                parameters.creakersDistBuilder.getDistribution(parameters.creakersLength),
//                parameters.waveDistBuilder.getDistribution()
        );
    }
}
