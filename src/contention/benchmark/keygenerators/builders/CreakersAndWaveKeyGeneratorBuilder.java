package contention.benchmark.keygenerators.builders;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.datamap.ArrayDataMap;
import contention.benchmark.Parameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.CreakersAndWaveKeyGenerator;
import contention.benchmark.keygenerators.parameters.CreakersAndWaveParameters;

public class CreakersAndWaveKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public CreakersAndWaveKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

    private DataMap data;

    @Override
    public void build(Parameters generalParameters) {
        super.build(generalParameters);

        data = new ArrayDataMap(generalParameters);
    }

    @Override
    public KeyGenerator getKeyGenerator() {
        CreakersAndWaveParameters parameters = (CreakersAndWaveParameters) this.parameters;

        return new CreakersAndWaveKeyGenerator(
                data,
                parameters
//                ,
//                parameters.creakersDistBuilder.getDistribution(parameters.creakersLength),
//                parameters.waveDistBuilder.getDistribution()
        );
    }
}
