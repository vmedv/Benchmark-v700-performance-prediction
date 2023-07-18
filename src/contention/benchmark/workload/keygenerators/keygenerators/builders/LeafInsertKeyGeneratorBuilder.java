package contention.benchmark.workload.keygenerators.keygenerators.builders;

import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilderOld;
import contention.benchmark.workload.keygenerators.keygenerators.impls.LeafInsertKeyGenerator;
import contention.benchmark.workload.datamap.impls.LeafInsertDataMap;

public class LeafInsertKeyGeneratorBuilder extends KeyGeneratorBuilderOld {

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
