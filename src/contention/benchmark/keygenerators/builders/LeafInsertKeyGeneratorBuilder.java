package contention.benchmark.keygenerators.builders;

import contention.benchmark.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.impls.LeafInsertKeyGenerator;
import contention.benchmark.datamap.impls.LeafInsertDataMap;

public class LeafInsertKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafInsertKeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        super(parameters);
    }

    private LeafInsertDataMap data = new LeafInsertDataMap(range);

//    @Override
//    public void initDataMaps() {
//        data = new LeafInsertDataMap(generalParameters);
//    }

    @Override
    public KeyGenerator build() {
        return new LeafInsertKeyGenerator(data, range);
    }
}
