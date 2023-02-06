package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.LeafsExtensionHandshakeKeyGenerator;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class LeafsExtensionHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public LeafsExtensionHandshakeKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        LeafsHandshakeParameters parameters = (LeafsHandshakeParameters) this.parameters;
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new LeafsExtensionHandshakeKeyGenerator(
                    parameters.range,
                    parameters.readDistBuilder.getDistribution(),
                    parameters.insertDistBuilder.getDistribution(),
                    parameters.eraseDistBuilder.getDistribution()
            );
        }

        LeafsExtensionHandshakeKeyGenerator.readData = switch (parameters.readDistBuilder.distributionType) {
            case ZIPF, SKEWED_SETS -> new SimpleKeyGeneratorData(parameters);
            default -> null;
        };

        // todo think about the intersection of sets
        if (parameters.readDistBuilder.distributionType == parameters.eraseDistBuilder.distributionType) {
            LeafsExtensionHandshakeKeyGenerator.eraseData = LeafsExtensionHandshakeKeyGenerator.readData;
        } else {
            LeafsExtensionHandshakeKeyGenerator.eraseData = switch (parameters.readDistBuilder.distributionType) {
                case ZIPF, SKEWED_SETS -> new SimpleKeyGeneratorData(parameters);
                default -> null;
            };
        }

        // to initialize deletedValue
//        keygens[0].nextErase();
        LeafsExtensionHandshakeKeyGenerator.deletedValue = new AtomicInteger(keygens[0].nextErase());

        return keygens;
    }
}
