package contention.abstractions;

import contention.benchmark.ThreadLoops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.ThreadLoops.parameters.TemporaryOperationsThreadLoopParameters;
import contention.benchmark.keygenerators.builders.*;
import contention.benchmark.keygenerators.parameters.*;

/**
 * Parameters of the Java version of the
 * Synchrobench benchmark.
 *
 * @author Vincent Gramoli
 */
public class Parameters {

    public int numThreads = 1,
            numPrefillThreads = 1,
            numMilliseconds = 5000,
            range = 2048,
            size = 1024,
            warmUp = 0,
            iterations = 1,
            afterFillRelaxMilliseconds = 5000;

    public boolean detailedStats = false;

    public boolean isNonShuffle = false;

    public String benchClassName = "skiplists.lockfree.NonBlockingFriendlySkipListMap";

    public ThreadLoopType threadLoopType = ThreadLoopType.DEFAULT;

    public KeyGeneratorType keygenType = KeyGeneratorType.DEFAULT_KEYGEN;

    public ThreadLoopParameters threadLoopParameters;


    public StringBuilder toStringBuilder() {
        StringBuilder result = new StringBuilder()
                .append("Benchmark parameters" + "\n" + "--------------------")
                .append("\n" + "  Detailed stats:          \t")
                .append((this.detailedStats ? "enabled" : "disabled"))
                .append("\n")
                .append("  Number of threads:       \t")
                .append(this.numThreads)
                .append("\n")
                .append("  Length:                  \t")
                .append(this.numMilliseconds)
                .append(" ms\n")
                .append("  Size:                    \t")
                .append(this.size)
                .append(" elts\n")
                .append("  Range:                   \t")
                .append(this.range)
                .append(" elts\n")
                .append(threadLoopParameters.toStringBuilder())
                .append("  WarmUp:                  \t")
                .append(this.warmUp)
                .append(" s\n")
                .append("  After fill relax time:   \t")
                .append(this.afterFillRelaxMilliseconds)
                .append(" ms\n")
                .append("  Iterations:              \t")
                .append(this.iterations)
                .append("\n")
                .append("  Benchmark:               \t")
                .append(this.benchClassName);

        if (isNonShuffle) {
            result.append("\n")
                    .append("  Data type:                \t")
                    .append("non-shuffle");
        }

        result.append("\n")
                .append("  Key Generator:           \t")
                .append(this.keygenType);

        return result;
    }

    public void build() {
        threadLoopParameters.build();
    }

    protected void parseArg(ParseArgument args) {
        String currentArg = args.getCurrent();

        try {
            switch (currentArg) {
                case "--verbose", "-v" -> this.detailedStats = true;
                case "--non-shuffle" -> this.isNonShuffle = true;
                default -> {
//                    String optionValue = args[++argNumber];
                    switch (currentArg) {
                        case "--thread-nums", "-t" -> this.numThreads = Integer.parseInt(args.getNext());
                        case "--prefill-thread-nums", "-pt" ->
                                this.numPrefillThreads = Integer.parseInt(args.getNext());
                        case "--duration", "-d" -> this.numMilliseconds = Integer.parseInt(args.getNext());
                        case "--size", "-i" -> this.size = Integer.parseInt(args.getNext());
                        case "--range", "-r" -> this.range = Integer.parseInt(args.getNext());
                        case "--Warmup", "-W" -> this.warmUp = Integer.parseInt(args.getNext());
                        case "--benchmark", "-b" -> this.benchClassName = args.getNext();
                        case "--iterations", "-n" -> this.iterations = Integer.parseInt(args.getNext());
                        case "--after-fill-relax-time", "-afr" ->
                                this.afterFillRelaxMilliseconds = Integer.parseInt(args.getNext());
                        default -> {
                            if (threadLoopParameters != null && !threadLoopParameters.parseArg(args)) {
                                System.err.println("Unexpected option: " + currentArg + ". Ignoring...");
                            }
                        }
                    }
                }
            }
        } catch (IndexOutOfBoundsException e) {
            System.err.println("Missing value after option: " + currentArg
                    + ". Ignoring...");
        } catch (NumberFormatException e) {
            System.err.println("Number expected after option:  "
                    + currentArg + ". Ignoring...");
        }
    }

    public static KeyGeneratorBuilder parse(String[] rowArgs) {
        ParseArgument args = new ParseArgument(rowArgs);
        KeyGeneratorBuilder keyGeneratorBuilder = parseThreadLoop(args);
        while (args.hasNext()) {
            keyGeneratorBuilder.parameters.parseArg(args);
            args.next();
        }
        keyGeneratorBuilder.parameters.build();
        return keyGeneratorBuilder;
    }

    private static KeyGeneratorBuilder parseKeyGenerator(ParseArgument args) {
        Parameters parameters;
        KeyGeneratorBuilder keyGeneratorBuilder;
        switch (args.getCurrent()) {
            case "-skewed-sets" -> {
                parameters = new SkewedSetsParameters();
                parameters.keygenType = KeyGeneratorType.SKEWED_SETS;

                keyGeneratorBuilder = new SkewedSetsKeyGeneratorBuilder(parameters);
            }
            case "-creakers-and-wave" -> {
                parameters = new CreakersAndWaveParameters();
                parameters.keygenType = KeyGeneratorType.CREAKERS_AND_WAVE;

                keyGeneratorBuilder = new CreakersAndWaveKeyGeneratorBuilder(parameters);
            }
            case "-temporary-skewed", "-temp-skewed" -> {
                parameters = new TemporarySkewedParameters();
                parameters.keygenType = KeyGeneratorType.TEMPORARY_SKEWED;

                keyGeneratorBuilder = new TemporarySkewedKeyGeneratorBuilder(parameters);
            }
            case "-leaf-insert" -> {
                parameters = new Parameters();
                parameters.keygenType = KeyGeneratorType.LEAF_INSERT;

                keyGeneratorBuilder = new LeafInsertKeyGeneratorBuilder(parameters);
            }
            case "-leafs-handshake" -> {
                parameters = new LeafsHandshakeParameters();
                parameters.keygenType = KeyGeneratorType.LEAFS_HANDSHAKE;

                keyGeneratorBuilder = new LeafsHandshakeKeyGeneratorBuilder(parameters);
            }
            case "-leafs-extension-handshake" -> {
                parameters = new LeafsHandshakeParameters();
                parameters.keygenType = KeyGeneratorType.LEAFS_EXTENSION_HANDSHAKE;

                keyGeneratorBuilder = new LeafsExtensionHandshakeKeyGeneratorBuilder(parameters);
            }
            default -> {
                parameters = new DefaultParameters();
                parameters.keygenType = KeyGeneratorType.DEFAULT_KEYGEN;

                return new DefaultKeyGeneratorBuilder(parameters);
            }
        }
        args.next();
        return keyGeneratorBuilder;
    }

    private static KeyGeneratorBuilder parseThreadLoop(ParseArgument args) {
        KeyGeneratorBuilder keyGeneratorBuilder;
        switch (args.getCurrent()) {
            case "-delete-leafs" -> {
                Parameters parameters = new Parameters();
                parameters.threadLoopType = ThreadLoopType.DELETE_LEAFS;
                parameters.keygenType = KeyGeneratorType.NONE;

                keyGeneratorBuilder = new NoneKeyGeneratorBuilder(parameters);
            }
            case "-temp-oper", "-temporary-operation" -> {
                args.next();
                keyGeneratorBuilder = parseKeyGenerator(args);
                keyGeneratorBuilder.parameters.threadLoopParameters = new TemporaryOperationsThreadLoopParameters();
                keyGeneratorBuilder.parameters.threadLoopType = ThreadLoopType.TEMPORARY_OPERATIONS;
            } case "-temp-oper-2", "-temporary-operation-2" -> {
                args.next();
                keyGeneratorBuilder = parseKeyGenerator(args);
                keyGeneratorBuilder.parameters.threadLoopParameters = new TemporaryOperationsThreadLoopParameters();
                keyGeneratorBuilder.parameters.threadLoopType = ThreadLoopType.TEMPORARY_OPERATIONS_2;
            }
            default -> {
                keyGeneratorBuilder = parseKeyGenerator(args);
                keyGeneratorBuilder.parameters.threadLoopParameters = new DefaultThreadLoopParameters();
            }
        }
        return keyGeneratorBuilder;
    }

}
