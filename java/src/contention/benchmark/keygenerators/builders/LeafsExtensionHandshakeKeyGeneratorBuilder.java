package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.data.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.LeafsExtensionHandshakeKeyGenerator;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

public class LeafsExtensionHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafsExtensionHandshakeKeyGeneratorBuilder(Parameters parameters) {
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
            keygens[threadNum] = new LeafsExtensionHandshakeKeyGenerator(
                    readData,
                    eraseData,
                    parameters,
                    parameters.readDistBuilder.getDistribution(),
                    parameters.insertDistBuilder.getDistribution(),
                    parameters.eraseDistBuilder.getDistribution()
            );
        }

        // to initialize deletedValue
//        keygens[0].nextErase();
//        LeafsExtensionHandshakeKeyGenerator.deletedValue = new AtomicInteger(keygens[0].nextErase());

        return keygens;
    }
}
