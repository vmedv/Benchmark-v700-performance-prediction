package contention.abstractions;

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
            numWriteAlls = 0,
            numSnapshots = 0,
            range = 2048,
            size = 1024,
            warmUp = 0,
            iterations = 1,
            afterFillRelaxMilliseconds = 5000;
    ;

    public boolean detailedStats = false;

    public String benchClassName = "skiplists.lockfree.NonBlockingFriendlySkipListMap";

    public WorkloadType workloadType = WorkloadType.REGULAR;

    public KeyGeneratorType keygenType = KeyGeneratorType.SIMPLE_KEYGEN;


    protected int argNumber;

    public void setArgNumber(int argNumber) {
        this.argNumber = argNumber;
    }

    protected void parseArg(String[] args) {
        String currentArg = args[argNumber++];

        try {
            if (currentArg.equals("--verbose")
                    || currentArg.equals("-v")) {
                this.detailedStats = true;
            } else {
                String optionValue = args[argNumber];
                switch (currentArg) {
                    case "--thread-nums", "-t" -> this.numThreads = Integer.parseInt(optionValue);
                    case "--prefill-thread-nums", "-pt" -> this.numPrefillThreads = Integer.parseInt(optionValue);
                    case "--duration", "-d" -> this.numMilliseconds = Integer
                            .parseInt(optionValue);
                    case "--updates", "-u" -> this.numWrites = Integer.parseInt(optionValue);
                    case "--writeAll", "-a" -> this.numWriteAlls = Integer.parseInt(optionValue);
                    case "--snapshots", "-s" -> this.numSnapshots = Integer.parseInt(optionValue);
                    case "--size", "-i" -> this.size = Integer.parseInt(optionValue);
                    case "--range", "-r" -> this.range = Integer.parseInt(optionValue);
                    case "--Warmup", "-W" -> this.warmUp = Integer.parseInt(optionValue);
                    case "--benchmark", "-b" -> this.benchClassName = optionValue;
                    case "--iterations", "-n" -> this.iterations = Integer.parseInt(optionValue);
                    case "--after-fill-relax-time", "-afr" -> this.afterFillRelaxMilliseconds = Integer.parseInt(optionValue);
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

    public void parse(String[] args) {
        while (argNumber < args.length) {
            parseArg(args);
            argNumber++;
        }
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
                .append(" ms\n")
                .append("  Write ratio:             \t")
                .append(this.numWrites)
                .append(" %\n")
                .append("  WriteAll ratio:          \t")
                .append(this.numWriteAlls)
                .append(" %\n")
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
        if (keygenType != KeyGeneratorType.NONE) {
            result.append("\n")
                    .append("  Key Generator:           \t")
                    .append(this.keygenType);
        }

        return result;
    }
}
