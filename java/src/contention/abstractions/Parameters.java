package contention.abstractions;

import contention.benchmark.ThreadLoops.workloads.parameters.TemporaryOperationsParameters;
import contention.benchmark.keygenerators.builders.*;
import contention.benchmark.keygenerators.parameters.*;

/**
 * Parameters of the Java version of the
 * Synchrobench benchmark.
 *
 * @author Vincent Gramoli
 */
public class Parameters {

    public static int numThreads = 1,
            numPrefillThreads = 1,
            numMilliseconds = 5000,
            numWrites = 40,
            numInsert = 20,
            numErase = 20,
            numWriteAlls = 0,
            numSnapshots = 0,
            range = 2048,
            size = 1024,
            warmUp = 0,
            iterations = 1,
            afterFillRelaxMilliseconds = 5000;


    public boolean detailedStats = false;

    public boolean isNonShuffle = false;

    public String benchClassName = "skiplists.lockfree.NonBlockingFriendlySkipListMap";

    public WorkloadType workloadType = WorkloadType.REGULAR;

    public KeyGeneratorType keygenType = KeyGeneratorType.SIMPLE_KEYGEN;


    protected int argNumber;
    protected String[] args;

    protected void parseArg() {
        String currentArg = args[argNumber];

        try {
            switch (currentArg) {
                case "--verbose", "-v" -> this.detailedStats = true;
                case "--non-shuffle" -> this.isNonShuffle = true;
                default -> {
                    String optionValue = args[++argNumber];
                    switch (currentArg) {
                        case "--thread-nums", "-t" -> this.numThreads = Integer.parseInt(optionValue);
                        case "--prefill-thread-nums", "-pt" -> this.numPrefillThreads = Integer.parseInt(optionValue);
                        case "--duration", "-d" -> this.numMilliseconds = Integer.parseInt(optionValue);
                        case "--updates", "-u" -> {
                            this.numWrites = Integer.parseInt(optionValue);
                            this.numInsert = this.numWrites / 2;
                            this.numErase = this.numWrites / 2;
                        }
                        case "--insert", "-ui" -> this.numInsert = Integer.parseInt(optionValue);
                        case "--erase", "-ue" -> this.numErase = Integer.parseInt(optionValue);
                        case "--writeAll", "-a" -> this.numWriteAlls = Integer.parseInt(optionValue);
                        case "--snapshots", "-s" -> this.numSnapshots = Integer.parseInt(optionValue);
                        case "--size", "-i" -> this.size = Integer.parseInt(optionValue);
                        case "--range", "-r" -> this.range = Integer.parseInt(optionValue);
                        case "--Warmup", "-W" -> this.warmUp = Integer.parseInt(optionValue);
                        case "--benchmark", "-b" -> this.benchClassName = optionValue;
                        case "--iterations", "-n" -> this.iterations = Integer.parseInt(optionValue);
                        case "--after-fill-relax-time", "-afr" ->
                                this.afterFillRelaxMilliseconds = Integer.parseInt(optionValue);
                        default -> System.err.println("Unexpected option: " + currentArg + ". Ignoring...");
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

    public void parse(String[] args, int startArgNumber) {
        this.argNumber = startArgNumber;
        this.parse(args);
    }

    public void parse(String[] args) {
        this.args = args;
        while (argNumber < args.length) {
            parseArg();
            argNumber++;
        }
        this.numWrites = this.numInsert + this.numErase;
        this.args = null;
        this.argNumber = 0;
    }

    public static KeyGeneratorBuilder parseWorkload(String[] args, int workloadIndex) {
        Parameters parameters;
        KeyGeneratorBuilder keyGeneratorBuilder;
        switch (args[workloadIndex]) {
            case "-skewed-sets" -> {
                parameters = new SkewedSetsParameters();
                parameters.keygenType = KeyGeneratorType.SKEWED_SETS;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new SkewedSetsKeyGeneratorBuilder(parameters);
            }
            case "-creakers-and-wave" -> {
                parameters = new CreakersAndWaveParameters();
                parameters.keygenType = KeyGeneratorType.CREAKERS_AND_WAVE;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new CreakersAndWaveKeyGeneratorBuilder(parameters);
            }
            case "-temporary-skewed", "-temp-skewed" -> {
                parameters = new TemporarySkewedParameters();
                parameters.keygenType = KeyGeneratorType.TEMPORARY_SKEWED;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new TemporarySkewedKeyGeneratorBuilder(parameters);
            }
            case "-delete-speed-test" -> {
                parameters = new Parameters();
                parameters.workloadType = WorkloadType.DELETE_SPEED_TEST;
                parameters.keygenType = KeyGeneratorType.NONE;
                parameters.numMilliseconds = 0;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new NoneKeyGeneratorBuilder(parameters);
            }
            case "-delete-leafs" -> {
                parameters = new Parameters();
                parameters.workloadType = WorkloadType.DELETE_LEAFS;
                parameters.keygenType = KeyGeneratorType.NONE;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new NoneKeyGeneratorBuilder(parameters);
            }
            case "-leaf-insert" -> {
                parameters = new Parameters();
                parameters.keygenType = KeyGeneratorType.LEAF_INSERT;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new LeafInsertKeyGeneratorBuilder(parameters);
            }
            case "-leafs-handshake" -> {
                parameters = new LeafsHandshakeParameters();
                parameters.keygenType = KeyGeneratorType.LEAFS_HANDSHAKE;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new LeafsHandshakeKeyGeneratorBuilder(parameters);
            }
            case "-leafs-extension-handshake" -> {
                parameters = new LeafsHandshakeParameters();
                parameters.keygenType = KeyGeneratorType.LEAFS_EXTENSION_HANDSHAKE;
                parameters.argNumber = workloadIndex + 1;

                keyGeneratorBuilder = new LeafsExtensionHandshakeKeyGeneratorBuilder(parameters);
            }
            case "-temp-oper", "-temporary-operation" -> {
                keyGeneratorBuilder = Parameters.parseWorkload(args, workloadIndex + 1);
                parameters = new TemporaryOperationsParameters();
                parameters.argNumber = keyGeneratorBuilder.parameters.argNumber;
                parameters.workloadType = WorkloadType.TEMPORARY_OPERATIONS;
                parameters.keygenType = keyGeneratorBuilder.parameters.keygenType;

                keyGeneratorBuilder.parameters = parameters;
            }
            default -> {
                parameters = new SimpleParameters();
                parameters.keygenType = KeyGeneratorType.SIMPLE_KEYGEN;
                parameters.argNumber = workloadIndex;

                keyGeneratorBuilder = new SimpleKeyGeneratorBuilder(parameters);
            }
        }
        return keyGeneratorBuilder;
    }

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
                .append(" ms\n");
        if (workloadType != WorkloadType.TEMPORARY_OPERATIONS) {
            result
                    .append("  Write ratio:             \t")
                    .append(this.numWrites)
                    .append(" %\n")
                    .append("  Insert ratio:            \t")
                    .append(this.numInsert)
                    .append(" %\n")
                    .append("  Erase ratio:             \t")
                    .append(this.numErase)
                    .append(" %\n")
                    .append("  WriteAll ratio:          \t")
                    .append(this.numWriteAlls)
                    .append(" %\n");
        }
        result
                .append("  Snapshot ratio:          \t")
                .append(this.numSnapshots)
                .append(" %\n")
                .append("  Size:                    \t")
                .append(this.size)
                .append(" elts\n")
                .append("  Range:                   \t")
                .append(this.range)
                .append(" elts\n")
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

        if (workloadType != WorkloadType.REGULAR) {
            result.append("\n")
                    .append("  Workload:                \t")
                    .append(this.workloadType);
        }
//        if (keygenType != KeyGeneratorType.NONE) {
//            result.append("\n")
//                    .append("  Key Generator:           \t")
//                    .append(this.keygenType);
//        }

        return result;
    }
}
