package contention.benchmark;

import contention.abstractions.DataStructure;
import contention.abstractions.ParseArgument;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoop;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopBuilder;
import contention.benchmark.stop.condition.StopCondition;
import contention.benchmark.stop.condition.Timer;

import java.lang.reflect.Method;
import java.util.*;

/**
 * Parameters of the Java version of the
 * Synchrobench benchmark.
 *
 * @author Vincent Gramoli
 */
public class Parameters {

    public int numThreads = 1;
//            numPrefillThreads = 1,
//            numMilliseconds = 5000,
   public int range = 2048;
//            size = 1024,
//            warmUp = 0,
//            iterations = 1,
//            afterFillRelaxMilliseconds = 5000;
    /**
     * a maxAwaitTime of 0 means to wait forever
     */
    public long maxAwaitTime = 0;

    public boolean detailedStats = false;


//    public String benchClassName = "skiplists.lockfree.NonBlockingFriendlySkipListMap";

//    public ThreadLoopType threadLoopType = ThreadLoopType.DEFAULT;

//    public KeyGeneratorType keygenType = KeyGeneratorType.DEFAULT_KEYGEN;

    public StopCondition stopCondition = new Timer(5000);

    public List<ThreadLoopBuilder> threadLoopBuilders = new ArrayList<>();
//    public List<ThreadLoopBuilder> prefillThreadLoopBuilders = new ArrayList<>();

    private NavigableMap<Integer, Integer> threadLoopIndexes;

//    public ThreadLoopParameters threadLoopParameters;


    public StringBuilder toStringBuilder() {
        StringBuilder params = new StringBuilder()
//                .append("Benchmark parameters" + "\n" + "--------------------")
                .append("  Detailed stats:          \t")
                .append((this.detailedStats ? "enabled" : "disabled"))
                .append("\n")
                .append("  Number of threads:       \t")
                .append(this.numThreads)
                .append("\n")
//                .append("  Length:                  \t")
//                .append(this.numMilliseconds)
//                .append(" ms\n")
                .append("  Stop condition:          \t")
                .append(stopCondition.toStringBuilder())
                .append("\n")
//                .append("  Size:                    \t")
//                .append(this.size)
//                .append(" elts\n")
                .append("  Range:                   \t")
                .append(this.range)
                .append(" elts\n");
//                .append("  WarmUp:                  \t")
//                .append(this.warmUp)
//                .append(" s\n")
//                .append("  After fill relax time:   \t")
//                .append(this.afterFillRelaxMilliseconds)
//                .append(" ms\n");
//                .append("  Iterations:              \t")
//                .append(this.iterations)
//                .append("\n")
//                .append("  Benchmark:               \t")
//                .append(this.benchClassName)
//                .append("\n");

        threadLoopBuilders.forEach(it -> params.append(it.toStringBuilder()).append("\n"));
        return params;
    }

    public void init() {
        threadLoopBuilders.forEach(it -> it.init(this));
//        threadLoopBuilders.forEach(it -> it.init(this));
//        threadLoopBuilders.parameter.init()
        threadLoopIndexes = new TreeMap<>();
        threadLoopIndexes.put(0, 0);
        for (int i = 0, sum = 0; i < threadLoopBuilders.size() - 1; i++) {
            sum += threadLoopBuilders.get(i).quantity;
            threadLoopIndexes.put(sum, i + 1);
        }
    }

    public void parseArg(ParseArgument args) {
        try {
            String currentArg = args.getCurrent();
            switch (currentArg) {
                case "--verbose", "-v" -> this.detailedStats = true;
                default -> {
//                    String optionValue = args[++argNumber];
                    switch (currentArg) {
                        case "--thread-nums", "-t" -> this.numThreads = Integer.parseInt(args.getNext());
//                        case "--prefill-thread-nums", "-pt" ->
//                                this.numPrefillThreads = Integer.parseInt(args.getNext());
//                        case "--duration", "-d" -> this.numMilliseconds = Integer.parseInt(args.getNext());
//                        case "--size", "-i" -> this.size = Integer.parseInt(args.getNext());
                        case "--range", "-r" -> this.range = Integer.parseInt(args.getNext());
//                        case "--Warmup", "-W" -> this.warmUp = Integer.parseInt(args.getNext());
//                        case "--benchmark", "-b" -> this.benchClassName = args.getNext();
//                        case "--iterations", "-n" -> this.iterations = Integer.parseInt(args.getNext());
//                        case "--after-fill-relax-time", "-afr" ->
//                                this.afterFillRelaxMilliseconds = Integer.parseInt(args.getNext());
                        default -> {
                            if (!threadLoopBuilders.get(0).parameters.parseArg(args)) {
                                System.err.println("Unexpected option: " + currentArg + ". Ignoring...");
                            }
                        }
                    }
                }
            }
        } catch (IndexOutOfBoundsException e) {
            System.err.println("Missing value after option: " + args.getCurrent()
                    + ". Ignoring...");
        } catch (NumberFormatException e) {
            System.err.println("Number expected after option:  "
                    + args.getCurrent() + ". Ignoring...");
        }
    }

    public static Parameters parse(ParseArgument args) {
        Parameters parameters = new Parameters();
        parameters.threadLoopBuilders.add(ThreadLoopBuilder.parseAndCreateThreadLoop(args)); //new ThreadLoopBuilder().parseThreadLoop(args));

        while (args.hasNext()) {
            parameters.parseArg(args);
            args.next();
        }
        parameters.init();
        return parameters;
    }

    public static Parameters parse(String[] rowArgs) {
        ParseArgument args = new ParseArgument(rowArgs);
        return parse(args);
    }

    public ThreadLoop getWorkload(int threadNum, DataStructure<Integer> dataStructure, Method[] methods) {
        int index = threadLoopIndexes.floorEntry(threadNum).getValue();
        return threadLoopBuilders.get(index).build(threadNum, dataStructure, methods);
    }

}
