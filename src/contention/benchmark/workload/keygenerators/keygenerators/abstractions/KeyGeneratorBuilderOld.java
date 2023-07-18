package contention.benchmark.workload.keygenerators.keygenerators.abstractions;

//import contention.benchmark.workload.keygenerators.parameters.*;

public class KeyGeneratorBuilderOld {
    public KeyGeneratorType type;
    public KeyGeneratorParameters parameters;
    public int range;
//    public Parameters generalParameters;

//    protected Map<String, DataMap> dataMaps;

    public KeyGeneratorBuilderOld() {
        this.type = KeyGeneratorType.DEFAULT_KEYGEN;
//        this.parameters = new DefaultParameters();
    }

    public KeyGeneratorBuilderOld(KeyGeneratorType type) {
        this.type = type;
    }

    public KeyGeneratorBuilderOld(KeyGeneratorParameters parameters) {
        this.parameters = parameters;
    }

    public KeyGeneratorBuilderOld setType(KeyGeneratorType type) {
        this.type = type;
        return this;
    }

    public KeyGeneratorBuilderOld setParameters(KeyGeneratorParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public void initParamters(int range) {
        this.range = range;
        parameters.init(range);
//        this.generalParameters = generalParameters;
//        initParamters(this.parameters, generalParameters);
    }

    public void initParamters(int range, KeyGeneratorParameters parameters) {
        this.parameters = parameters;
        parameters.init(range);
//        this.generalParameters = generalParameters;
    }

    public KeyGenerator build() {
        return null;
//        return switch (type) {
//            case DEFAULT_KEYGEN -> {
//                DefaultParameters parameters = (DefaultParameters) this.parameters;
//
//                yield new DefaultKeyGenerator(
//                        parameters.dataMapBuilder.build(range),
//                        parameters.distributionBuilder.build(range)
//                );
//            }
//            case SKEWED_SETS -> new SkewedSetsKeyGenerator(
//                    (SkewedSetsParameters) this.parameters, range);
//            case TEMPORARY_SKEWED -> {
//                TemporarySkewedParameters parameters = (TemporarySkewedParameters) this.parameters;
//
//                yield new TemporarySkewedKeyGenerator(parameters.dataMapBuilder.build(range), parameters, range);
//            }
//            case CREAKERS_AND_WAVE -> {
//                CreakersAndWaveParameters parameters = (CreakersAndWaveParameters) this.parameters;
//
////                yield new CreakersAndWaveKeyGenerator(parameters.dataMapBuilder.build(range), parameters);
//            }
//            case LEAF_INSERT -> null;
//            case LEAFS_HANDSHAKE -> null;
//            case LEAFS_EXTENSION_HANDSHAKE -> null;
//            case NONE -> null;
//        };
    }

//    public static KeyGeneratorBuilderOld parseKeyGenerator(ParseArgument args) {
//        return switch (args.getCurrent()) {
//            case "-skewed-sets" -> {
//                args.next();
//                yield new KeyGeneratorBuilderOld(KeyGeneratorType.SKEWED_SETS).setParameters(new SkewedSetsParameters());
//            }
//            case "-creakers-and-wave" -> {
//                args.next();
//                yield new KeyGeneratorBuilderOld(KeyGeneratorType.CREAKERS_AND_WAVE).setParameters(new CreakersAndWaveParameters());
//            }
//            case "-temporary-skewed", "-temp-skewed" -> {
//                args.next();
//                yield new KeyGeneratorBuilderOld(KeyGeneratorType.TEMPORARY_SKEWED).setParameters(new TemporarySkewedParameters());
//            }
//            case "-leaf-insert" -> {
//                args.next();
//                yield new KeyGeneratorBuilderOld(KeyGeneratorType.LEAF_INSERT);
//            }
//            case "-leafs-handshake" -> {
//                args.next();
//                yield new KeyGeneratorBuilderOld(KeyGeneratorType.LEAFS_HANDSHAKE).setParameters(new LeafsHandshakeParameters());
//            }
//            case "-leafs-extension-handshake" -> {
//                args.next();
//                yield new KeyGeneratorBuilderOld(KeyGeneratorType.LEAFS_EXTENSION_HANDSHAKE).setParameters(new LeafsHandshakeParameters());
//            }
//            default -> new KeyGeneratorBuilderOld(KeyGeneratorType.DEFAULT_KEYGEN).setParameters(new DefaultParameters());
//        };
//    }


}
