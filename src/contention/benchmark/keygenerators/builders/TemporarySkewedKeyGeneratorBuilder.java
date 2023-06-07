package contention.benchmark.keygenerators.builders;

import contention.benchmark.Parameters;
import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.TemporarySkewedKeyGenerator;
import contention.benchmark.datamap.ArrayDataMap;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public TemporarySkewedKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

    private DataMap data;
    private Parameters generalParameters;

    @Override
    public void build(Parameters generalParameters) {
        super.build(generalParameters);

        data = new ArrayDataMap(generalParameters);
    }

    @Override
    public KeyGenerator getKeyGenerator() {
        TemporarySkewedParameters parameters = (TemporarySkewedParameters) this.parameters;

        return new TemporarySkewedKeyGenerator(
                data,
                parameters,
                generalParameters.range
//                ,
//                hotDists,
//                new UniformDistribution(parameters.range)
        );
    }

}
