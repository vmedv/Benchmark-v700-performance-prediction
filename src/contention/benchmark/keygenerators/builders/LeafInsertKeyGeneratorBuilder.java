package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.Parameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.LeafInsertKeyGenerator;
import contention.benchmark.datamap.LeafInsertDataMap;

public class LeafInsertKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafInsertKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

    private LeafInsertDataMap data;
    private Parameters generalParameters;

    @Override
    public void build(Parameters generalParameters) {
        super.build(generalParameters);

        data = new LeafInsertDataMap(generalParameters);
        this.generalParameters = generalParameters;
    }

    @Override
    public KeyGenerator getKeyGenerator() {
        return new LeafInsertKeyGenerator(data, generalParameters.range);
    }
}
