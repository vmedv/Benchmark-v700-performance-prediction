package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.*;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

public class LeafsHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafsHandshakeKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        LeafsHandshakeParameters parameters = (LeafsHandshakeParameters) this.parameters;
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        KeyGeneratorData readData = new KeyGeneratorData(parameters);
        KeyGeneratorData eraseData;
        // todo think about the intersection of sets
        if (parameters.readDistBuilder.distributionType == parameters.eraseDistBuilder.distributionType) {
            eraseData = readData;
        } else {
            eraseData = new KeyGeneratorData(parameters);
        }

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new LeafsHandshakeKeyGenerator(
                    readData,
                    eraseData,
                    parameters,
                    parameters.readDistBuilder.getDistribution(parameters.range),
                    parameters.insertDistBuilder.getDistribution(),
                    parameters.eraseDistBuilder.getDistribution(parameters.range)
            );
        }

        // to initialize deletedValue
//        keygens[0].nextErase();
//        LeafsHandshakeKeyGenerator.deletedValue = new AtomicInteger(keygens[0].nextErase());

        return keygens;
    }

}

