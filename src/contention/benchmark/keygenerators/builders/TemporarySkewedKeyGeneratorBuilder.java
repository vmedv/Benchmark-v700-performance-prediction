package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.TemporarySkewedKeyGenerator;
import contention.benchmark.datamap.impls.ArrayDataMap;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public TemporarySkewedKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

//    @Override
//    public void initDataMaps() {
//        dataMaps.put("data", new ArrayDataMap(generalParameters));
//    }

    @Override
    public KeyGenerator build() {
        TemporarySkewedParameters parameters = (TemporarySkewedParameters) this.parameters;

        return new TemporarySkewedKeyGenerator(
//                dataMaps.get("data"),
                parameters.dataMapBuilder.build(range),
                parameters,
                range
//                ,
//                hotDists,
//                new UniformDistribution(parameters.range)
        );
    }

}
