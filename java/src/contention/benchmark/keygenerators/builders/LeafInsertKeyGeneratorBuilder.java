package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.LeafInsertKeyGenerator;
import contention.benchmark.keygenerators.data.LeafInsertKeyGeneratorData;

public class LeafInsertKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafInsertKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        LeafInsertKeyGenerator.data = new LeafInsertKeyGeneratorData(parameters);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new LeafInsertKeyGenerator(parameters.range);
        }

        return keygens;
    }
}
