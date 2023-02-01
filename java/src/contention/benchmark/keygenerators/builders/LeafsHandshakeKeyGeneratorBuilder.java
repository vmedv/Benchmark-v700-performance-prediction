package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.*;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

import java.util.concurrent.atomic.AtomicInteger;

public class LeafsHandshakeKeyGeneratorBuilder extends KeyGeneratorBuilder {

    @Override
    public KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        LeafsHandshakeParameters parameters = (LeafsHandshakeParameters) rawParameters;
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new LeafsHandshakeKeyGenerator(
                    parameters.range,
                    parameters.readDistBuilder.getDistribution(parameters.range),
                    parameters.insertDistBuilder.getDistribution(),
                    parameters.eraseDistBuilder.getDistribution(parameters.range)
            );
        }

        LeafsHandshakeKeyGenerator.readData = switch (parameters.readDistBuilder.distributionType) {
            case ZIPF, SKEWED_SETS -> new SimpleKeyGeneratorData(parameters);
            default -> null;
        };

        // todo think about the intersection of sets
        if (parameters.readDistBuilder.distributionType == parameters.eraseDistBuilder.distributionType) {
            LeafsHandshakeKeyGenerator.eraseData = LeafsHandshakeKeyGenerator.readData;
        } else {
            LeafsHandshakeKeyGenerator.eraseData = switch (parameters.readDistBuilder.distributionType) {
                case ZIPF, SKEWED_SETS -> new SimpleKeyGeneratorData(parameters);
                default -> null;
            };
        }

        // to initialize deletedValue
//        keygens[0].nextErase();
        LeafsHandshakeKeyGenerator.deletedValue = new AtomicInteger(keygens[0].nextErase());

        return keygens;
    }

}

