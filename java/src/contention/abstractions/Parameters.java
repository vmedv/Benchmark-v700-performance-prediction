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
            iterations = 1;

    public boolean detailedStats = false;

    public String benchClassName = "skiplists.lockfree.NonBlockingFriendlySkipListMap";

    public KeyGeneratorType keygenType = KeyGeneratorType.SIMPLE_KEYGEN;


    protected int argNumber;

    public void parse(String[] args) {
        String currentArg = args[argNumber++];

        try {
            if (currentArg.equals("--verbose")
                    || currentArg.equals("-v")) {
                this.detailedStats = true;
            } else {
                String optionValue = args[argNumber];
                switch (currentArg) {
                    case "--thread-nums":
                    case "-t":
                        this.numThreads = Integer.parseInt(optionValue);
                        break;
                    case "--prefill-thread-nums":
                    case "-pt":
                        this.numPrefillThreads = Integer.parseInt(optionValue);
                        break;
                    case "--duration":
                    case "-d":
                        this.numMilliseconds = Integer
                                .parseInt(optionValue);
                        break;
                    case "--updates":
                    case "-u":
                        this.numWrites = Integer.parseInt(optionValue);
                        break;
                    case "--writeAll":
                    case "-a":
                        this.numWriteAlls = Integer.parseInt(optionValue);
                        break;
                    case "--snapshots":
                    case "-s":
                        this.numSnapshots = Integer.parseInt(optionValue);
                        break;
                    case "--size":
                    case "-i":
                        this.size = Integer.parseInt(optionValue);
                        break;
                    case "--range":
                    case "-r":
                        this.range = Integer.parseInt(optionValue);
                        break;
                    case "--Warmup":
                    case "-W":
                        this.warmUp = Integer.parseInt(optionValue);
                        break;
                    case "--benchmark":
                    case "-b":
                        this.benchClassName = optionValue;
                        break;
                    case "--iterations":
                    case "-n":
                        this.iterations = Integer.parseInt(optionValue);
                        break;
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

    public StringBuilder toStringBuilder() {
        return new StringBuilder()
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
                .append("  Iterations:              \t")
                .append(this.iterations)
                .append("\n")
                .append("  Benchmark:               \t")
                .append(this.benchClassName);
    }
}
