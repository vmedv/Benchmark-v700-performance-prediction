package contention.benchmark.keygenerators.builders;

import contention.benchmark.datamap.abstractions.DataMap;
import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.Parameters;
import contention.benchmark.datamap.*;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.DefaultKeyGenerator;
import contention.benchmark.keygenerators.parameters.DefaultParameters;

public class DefaultKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public DefaultKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }


    private DataMap data;
    private Parameters generalParameters;

    @Override
    public void build(Parameters generalParameters) {
        super.build(generalParameters);

        this.generalParameters = generalParameters;
        DefaultParameters parameters = (DefaultParameters) this.parameters;

        data = switch (parameters.distributionBuilder.distributionType) {
            case ZIPF, SKEWED_UNIFORM -> new ArrayDataMap(generalParameters);
            default -> new IdDataMap();
        };
    }

    @Override
    public KeyGenerator getKeyGenerator() {
        DefaultParameters parameters = (DefaultParameters) this.parameters;

        return new DefaultKeyGenerator(
                data,
                parameters.distributionBuilder.getDistribution(generalParameters.range)
        );
    }
}
