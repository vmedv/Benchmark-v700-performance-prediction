package contention.benchmark.keygenerators.abstractions;

import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.keygenerators.builders.*;
import contention.benchmark.keygenerators.parameters.*;

public abstract class KeyGeneratorBuilder {
//    public KeyGeneratorType keyGeneratorType;
    public KeyGeneratorParameters parameters;

    public KeyGeneratorBuilder(KeyGeneratorParameters parameters) {
        this.parameters = parameters;
    }

    public void build(Parameters generalParameters) {
        parameters.build(generalParameters);
    }
    public abstract KeyGenerator getKeyGenerator();

    public static KeyGeneratorBuilder parseKeyGenerator(ParseArgument args) {
        KeyGeneratorParameters parameters;
        KeyGeneratorBuilder keyGeneratorBuilder;
        switch (args.getCurrent()) {
            case "-skewed-sets" -> {
                parameters = new SkewedSetsParameters();
//                parameters.keygenType = KeyGeneratorType.SKEWED_SETS;

                keyGeneratorBuilder = new SkewedSetsKeyGeneratorBuilder(parameters);
            }
            case "-creakers-and-wave" -> {
                parameters = new CreakersAndWaveParameters();
//                parameters.keygenType = KeyGeneratorType.CREAKERS_AND_WAVE;

                keyGeneratorBuilder = new CreakersAndWaveKeyGeneratorBuilder(parameters);
            }
            case "-temporary-skewed", "-temp-skewed" -> {
                parameters = new TemporarySkewedParameters();
//                parameters.keygenType = KeyGeneratorType.TEMPORARY_SKEWED;

                keyGeneratorBuilder = new TemporarySkewedKeyGeneratorBuilder(parameters);
            }
            case "-leaf-insert" -> {
//                parameters = new Parameters();
//                parameters.keygenType = KeyGeneratorType.LEAF_INSERT;

                keyGeneratorBuilder = new LeafInsertKeyGeneratorBuilder(null);
            }
            case "-leafs-handshake" -> {
                parameters = new LeafsHandshakeParameters();
//                parameters.keygenType = KeyGeneratorType.LEAFS_HANDSHAKE;

                keyGeneratorBuilder = new LeafsHandshakeKeyGeneratorBuilder(parameters);
            }
            case "-leafs-extension-handshake" -> {
                parameters = new LeafsHandshakeParameters();
//                parameters.keygenType = KeyGeneratorType.LEAFS_EXTENSION_HANDSHAKE;

                keyGeneratorBuilder = new LeafsExtensionHandshakeKeyGeneratorBuilder(parameters);
            }
            default -> {
                parameters = new DefaultParameters();
//                parameters.keygenType = KeyGeneratorType.DEFAULT_KEYGEN;

                return new DefaultKeyGeneratorBuilder(parameters);
            }
        }
        args.next();
        return keyGeneratorBuilder;
    }


}
