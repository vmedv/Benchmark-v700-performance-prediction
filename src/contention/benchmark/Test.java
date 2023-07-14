package contention.benchmark;

import contention.abstractions.*;

import java.io.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.Random;
//import javafx.util.Pair;

import contention.benchmark.ThreadLoops.abstractions.ThreadLoopAbstract;
import contention.benchmark.datasctucrure.MapDataStructure;
import contention.benchmark.datasctucrure.IntSetDataStructure;
import contention.benchmark.datasctucrure.SortedSetDataStructure;
import contention.benchmark.json.JsonConverter;
import contention.benchmark.tools.Pair;

import static contention.benchmark.tools.StringFormat.formatDouble;

/**
 * Synchrobench-java, a benchmark to evaluate the implementations of
 * high level abstractions including Map and Set.
 *
 * @author Vincent Gramoli
 */
public class Test {

    public static final String VERSION = "11-17-2014";


    public enum Type {
        INTSET, MAP, SORTEDSET;
    }

    /**
     * The array of threads executing the benchmark
     */
    private Thread[] threads;

    /**
     * The array of runnable thread codes
     */
    private ThreadLoopAbstract[] threadLoops;
    //    private ThreadLoop threadLoops;
    private double elapsedTime;
    /**
     * The observed duration of the benchmark
     */

    private ThreadStatistic curStats = new ThreadStatistic();
    public BenchStatistic[] benchStatistics;
    /**
     * The throughput
     */
    private double[] throughput = null;
    /**
     * The iteration
     */
    private int currentIteration = 0;
    /**
     * The total number of operations for all threads
     */
//    private long total = 0;

    /**
     * The total number of successful operations for all threads
     */
//    private long numAdd = 0;
//    private long numRemove = 0;
//    private long numAddAll = 0;
//    private long numRemoveAll = 0;
//    private long numSize = 0;
//    private long numContains = 0;
    /**
     * The total number of failed operations for all threads
     */
//    private long failures = 0;
    /**
     * The total number of aborts
     */
//    private long aborts = 0;
    /**
     * The instance of the benchmark
     */
    private Type benchType = null;
    private BenchParameters parameters;
    //    private Parameters testParameters;
//    private Parameters prefillParameters;
//    private Parameters warmUpParameters;
    private DataStructure<Integer> dataStructure;
    /**
     * The benchmark methods
     */
    private Method[] methods;
//    private long nodesTraversed;
//
//    public long structMods;
//    private long getCount;
    /**
     * The thread-private PRNG
     */
    final private static ThreadLocal<Random> s_random = new ThreadLocal<Random>() {
        @Override
        protected synchronized Random initialValue() {
            return new Random();
        }
    };

    //TODO delete parameters


    /**
     * Initialize the benchmark
     *
     * @param benchName the class name of the benchmark
     * @return the instance of the initialized corresponding benchmark
     */
    @SuppressWarnings("unchecked")
    public void instanciateAbstraction(String benchName) {
        try {
            Class<CompositionalMap<Integer, Integer>> benchClass = (Class<CompositionalMap<Integer, Integer>>) Class
                    .forName(benchName);
            Constructor<CompositionalMap<Integer, Integer>> c = benchClass
                    .getConstructor();
            methods = benchClass.getDeclaredMethods();

            if (CompositionalIntSet.class.isAssignableFrom(benchClass)) {
                dataStructure = new IntSetDataStructure((CompositionalIntSet) c.newInstance());
//                setBench = (CompositionalIntSet) c.newInstance();
                benchType = Type.INTSET;
            } else if (CompositionalMap.class.isAssignableFrom(benchClass)) {
                dataStructure = new MapDataStructure<>(c.newInstance());

//                mapBench = (CompositionalMap<Integer, Integer>) c.newInstance();
                benchType = benchType != null ? benchType : Type.MAP;
            } else if (CompositionalSortedSet.class.isAssignableFrom(benchClass)) {
                dataStructure = new SortedSetDataStructure<>((CompositionalSortedSet<Integer>) c.newInstance());
//                sortedBench = (CompositionalSortedSet<Integer>) c.newInstance();
                benchType = Type.SORTEDSET;
            }

        } catch (Exception e) {
            System.err.println("Cannot find benchmark class: " + benchName);
            System.exit(-1);
        }
    }


    /**
     * Creates as many threads as requested
     */
//    private void initThreads() {
////        KeyGenerator[] keygens = keyGeneratorBuilder.generateKeyGenerators();
//
//        threads = new Thread[parameters.numThreads];
//        threadLoops = new ThreadLoop[parameters.numThreads];
//
//        for (int threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
//            threadLoops[threadNum] = parameters.getWorkload(threadNum, dataStructure, methods);
//
//            threads[threadNum] = new Thread(threadLoops[threadNum]);
//        }
//
//    }

    /**
     * Creates as many threads as requested
     */
    private Pair<Thread[], ThreadLoopAbstract[]> initThreads(Parameters parameters) {
        Thread[] threads = new Thread[parameters.numThreads];
        ThreadLoopAbstract[] threadLoops = new ThreadLoopAbstract[parameters.numThreads];

        for (int threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            threadLoops[threadNum] = (ThreadLoopAbstract) parameters.getWorkload(threadNum, dataStructure, methods);

            threads[threadNum] = new Thread(threadLoops[threadNum]);
        }

        return new Pair<>(threads, threadLoops);
    }

    /**
     * Constructor sets up the benchmark by reading parameters and creating
     * threads
     *
     * @param args the arguments of the command-line
     */
    public Test(String[] args) throws InterruptedException {
        printHeader();
        try {
            parseCommandLineParameters(args);
        } catch (Exception e) {
            System.err.println("Cannot parse parameters.");
            e.printStackTrace();
        }
        instanciateAbstraction(parameters.benchClassName);
        this.throughput = new double[parameters.iterations];
    }


    /**
     * The instance of the benchmark
     */
    public Test(BenchParameters parameters) {
        this.parameters = parameters;
        instanciateAbstraction(parameters.benchClassName);
        this.throughput = new double[parameters.iterations];
        this.benchStatistics = new BenchStatistic[parameters.iterations];
    }

    /**
     * Execute the main thread that starts and terminates the benchmark threads
     *
     * @throws InterruptedException
     */
//    private void execute(int milliseconds, boolean maint) throws InterruptedException {
//        long startTime;
//        fill(parameters.range, parameters.size);
//        Thread.sleep(parameters.afterFillRelaxMilliseconds);
//
//        startTime = System.currentTimeMillis();
//        for (Thread thread : threads)
//            thread.start();
//
////        try {
////            Thread.sleep(milliseconds);
////        } finally {
////            for (ThreadLoop threadLoop : threadLoops)
////                threadLoop.stopThread();
////        }
////        for (Thread thread : threads)
////            thread.join();
//
//        for (Thread thread : threads) {
//            if (parameters.maxAwaitTime == 0) {
//                thread.join();
//            } else {
//                long awaitTime = Math.max(1, parameters.maxAwaitTime + startTime - System.currentTimeMillis());
//                System.out.println("Await time: " + awaitTime);
//                thread.join(awaitTime);
//                if (thread.isAlive()) {
//                    System.err.println("The thread " + thread.getName()
//                            + " was not terminated after the expiration of time. The max await time is "
//                            + parameters.maxAwaitTime + " millis. "
//                            + "The thread will be interrupted. ");
//                    thread.interrupt();
//
//                }
//            }
//        }
//
//        long endTime = System.currentTimeMillis();
//        elapsedTime = ((double) (endTime - startTime)) / 1000.0;
//    }
    private void execute(Parameters parameters, Thread[] threads) throws InterruptedException {
        long startTime;
//        fill(parameters.range, parameters.size);
//        Thread.sleep(parameters.afterFillRelaxMilliseconds);

        startTime = System.currentTimeMillis();
        parameters.stopCondition.start(parameters);
        for (Thread thread : threads)
            thread.start();

//        try {
//            Thread.sleep(milliseconds);
//        } finally {
//            for (ThreadLoop threadLoop : threadLoops)
//                threadLoop.stopThread();
//        }
//        for (Thread thread : threads)
//            thread.join();

        for (Thread thread : threads) {
            if (parameters.maxAwaitTime == 0) {
                thread.join();
            } else {
                long awaitTime = Math.max(1, parameters.maxAwaitTime + startTime - System.currentTimeMillis());
//                System.out.println("Await time: " + awaitTime);
                thread.join(awaitTime);
                if (thread.isAlive()) {
//                    System.err.println("The thread " + thread.getName()
//                            + " was not terminated after the expiration of time. The max await time is "
//                            + parameters.maxAwaitTime + " millis. ");
//                            + "The thread will be interrupted. ");
//                    thread.interrupt();
                    throw new RuntimeException("The thread " + thread.getName()
                            + " was not terminated after the expiration of time. The max await time is "
                            + parameters.maxAwaitTime + " millis. ");
                }
            }
        }

        long endTime = System.currentTimeMillis();
        elapsedTime = ((double) (endTime - startTime)) / 1000.0;
    }

    public void clear() {
        dataStructure.clear();
    }

    public static Parameters parseJson(String jsonParameters) {
        return JsonConverter.fromJson(jsonParameters);
    }

    public static Parameters parseCommandLine(String[] args) {
        return Parameters.parse(args);
    }

    public void run() throws InterruptedException {
        boolean firstIteration = true;

        for (int i = 0; i < parameters.iterations; i++) {
            if (!firstIteration) {
                // give time to the JIT
                Thread.sleep(100);
                resetStats();
                clear();
                org.deuce.transaction.estmstats.Context.threadIdCounter.set(0);
            }


            printStage("Prefill stage");

            Pair<Thread[], ThreadLoopAbstract[]> prefillInits = initThreads(parameters.prefill);
            Thread[] prefillThreads = prefillInits.first;
            threadLoops = prefillInits.second;
            execute(parameters.prefill, prefillThreads);


            printStage("WarmUp stage");

            Pair<Thread[], ThreadLoopAbstract[]> warmUpInits = initThreads(parameters.warmUp);
            Thread[] warmUpThreads = warmUpInits.first;
            threadLoops = warmUpInits.second;
            execute(parameters.prefill, warmUpThreads);

//            Thread[] warmUpThreads = initThreads(parametersters.warmUp).first;
//            execute(parameters.warmUp, warmUpThreads);

            if (parameters.test.detailedStats)
                recordPreliminaryStats();

//            clear();
            resetStats();

            long size = dataStructure.size();


            printStage("Test stage");

            Pair<Thread[], ThreadLoopAbstract[]> testInits = initThreads(parameters.test);
            Thread[] testThreads = testInits.first;
            threadLoops = testInits.second;

            execute(parameters.test, testThreads);

            if (dataStructure.getDataStructure() instanceof MaintenanceAlg) {
                ((MaintenanceAlg) dataStructure.getDataStructure()).stopMaintenance();
                curStats.structMods += ((MaintenanceAlg) dataStructure.getDataStructure()).getStructMods();
            }

            printBasicStats(size);
            if (parameters.test.detailedStats)
                printDetailedStats();

            firstIteration = false;
            currentIteration++;
        }

        if (parameters.iterations > 1) {
            printIterationStats();
        }

//        assert dataStructure.size() == 0 : "Warmup corrupted the data structure, rerun with -W 0.";
    }


    public static String readJsonFile(String fileName) throws IOException {
        StringBuilder jsonStringBuilder = new StringBuilder();
        try (BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
            String line;
            while ((line = reader.readLine()) != null)
                jsonStringBuilder.append(line);
        } catch (IOException e) {
            System.err.println("An error occurred while reading the file: \"" + fileName + "\".");
            throw e;
        }
        return jsonStringBuilder.toString();
    }

    public static <T> void writeJsonFile(T t, String fileName) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(fileName))) {
            String json = JsonConverter.toJson(t);
            writer.write(json);
        } catch (IOException e) {
            System.err.println("An error occurred while writing to the file: \"" + fileName + "\".");
            throw e;
        }
    }

    public static Parameters parseJsonFile(String fileName) throws IOException {
        return parseJson(readJsonFile(fileName));
    }

    public static void main(String[] rowArgs) throws IOException, InterruptedException {
        ParseArgument args = new ParseArgument(rowArgs);
        Test test;

        BenchParameters parameters = new BenchParameters();
        boolean needInit = true;
        String resultStatisticFileName = null;

        while (args.hasNext()) {
            switch (args.getCurrent()) {
                case "-without-init" -> needInit = false;
                case "-prefill" -> parameters.prefill = parseJsonFile(args.getNext());
                case "-warm-up" -> parameters.warmUp = parseJsonFile(args.getNext());
                case "-test" -> parameters.test = parseJsonFile(args.getNext());
                case "-create-default-prefill" -> parameters.createDefaultPrefill();
                case "-json-file" ->
                        parameters = JsonConverter.fromJsonT(readJsonFile(args.getNext()), BenchParameters.class);
                case "-result-file" -> resultStatisticFileName = args.getNext();
                default -> {
                    System.err.println("Unexpected option: " + args.getCurrent() + ". Ignoring...");
                    args.next();
                }
            }
        }

        if (needInit) {
            parameters.init();
        }

        test = new Test(parameters);

        printHeader();
        printStage("Prefill parameters");
        test.printParams(test.parameters.prefill);

        if (parameters.warmUp.numThreads != 0) {
            printStage("WarmUp parameters");
            test.printParams(test.parameters.test);
        } else {
            printStage("Without WarmUp");
        }

        printStage("Test parameters");
        test.printParams(test.parameters.test);

        test.run();

        if (resultStatisticFileName != null) {
            writeJsonFile(test.benchStatistics, resultStatisticFileName);
        }
    }

    /* ---------------- Input/Output -------------- */

    /**
     * Parse the parameters on the command line
     */
    private void parseCommandLineParameters(String[] args) {

        switch (args[0]) {
            case "--help", "-h" -> {
                printUsage();
                System.exit(0);
            }
        }

        parameters.test = Parameters.parse(args);

//        assert (testParameters.range >= testParameters.size);
//        if (parameters.range != 2 * parameters.size)
//            System.err
//                    .println("Note that the value range is not twice "
//                            + "the initial size, thus the size expectation varies at runtime.");
    }

    /**
     * Print a 80 character line filled with the same marker character
     *
     * @param ch the marker character
     */
    private static void printLine(char ch) {
        System.out.println(String.valueOf(ch).repeat(79));
    }

    /**
     * Print the header message on the standard output
     */
    private static void printHeader() {
        String header = "Synchrobench-java\n"
                + "A benchmark-suite to evaluate synchronization techniques";
        printLine('-');
        System.out.println(header);
        printLine('-');
        System.out.println();
    }

    private static void printStage(String stageName) {
        printLine('-');
        System.out.println(stageName);
        printLine('-');
        System.out.println();
    }

    /**
     * Print the benchmark usage on the standard output
     */
    private void printUsage() {
        String syntax = "Usage:\n"
                + "java synchrobench.benchmark.Test [options] [-- stm-specific options]\n\n"
                + "Options:\n"
                + "\t-v            -- print detailed statistics (default: "
                + parameters.test.detailedStats
                + ")\n"
                + "\t-t thread-num -- set the number of threads (default: "
                + parameters.test.numThreads
                + ")\n"
//                + "\t-d duration   -- set the length of the benchmark, in milliseconds (default: "
//                + parameters.test.numMilliseconds
//                + ")\n"
//                + "\t-u updates    -- set the percentage of updates (default: "
//                + ((DefaultThreadLoopParameters) parameters.threadLoopParameters).numWrites
//                + ")\n"
//                + "\t-ue inserts   -- set the percentage of threads (default: "
//                + ((DefaultThreadLoopParameters) parameters.threadLoopParameters).numInsert
//                + ")\n"
//                + "\t-ui removes   -- set the percentage of erases (default: "
//                + ((DefaultThreadLoopParameters) parameters.threadLoopParameters).numRemove
//                + ")\n"
//                + "\t-a writeAll   -- set the percentage of composite updates (default: "
//                + ((DefaultThreadLoopParameters) parameters.threadLoopParameters).numWriteAlls
//                + ")\n"
//                + "\t-s snapshot   -- set the percentage of composite read-only operations (default: "
//                + ((DefaultThreadLoopParameters) parameters.threadLoopParameters).numSnapshots
//                + ")\n"
                + "\t-r range      -- set the element range (default: "
                + parameters.test.range
                + ")\n"
//                + "\t-b benchmark  -- set the benchmark (default: "
//                + parameters.test.benchClassName
//                + ")\n"
//                + "\t-i size       -- set the datastructure initial size (default: "
//                + parameters.test.size
//                + ")\n"
                + "\t-n iterations -- set the bench iterations in the same JVM (default: "
                + parameters.iterations
                + ").";
//                + "\t-W warmup     -- set the JVM warmup length, in seconds (default: "
//                + testParameters.warmUp + ").";
        System.err.println(syntax);
    }

    /**
     * Print the parameters that have been given as an input to the benchmark
     */
    private void printParams(Parameters parameters) {
        System.out.println(parameters.toStringBuilder());
    }

    /**
     * Print the statistics on the standard output
     */
    private void printBasicStats(long prefillSize) {
        BenchStatistic curBenchStats = new BenchStatistic();
        curBenchStats.prefillSize = prefillSize;
        curBenchStats.threadStatistics = new ThreadStatistic[threadLoops.length];
        for (int i = 0; i < threadLoops.length; i++) {
            curStats.add(threadLoops[i].stats);
            curBenchStats.threadStatistics[i] = threadLoops[i].stats;
        }
        curBenchStats.elapsedTime = elapsedTime;
        curBenchStats.throughput = ((double) curStats.total / elapsedTime);
        curBenchStats.commonStatistic = curStats;
        curBenchStats.effectiveUpdates = curStats.numAdd + curStats.numRemove + curStats.numAddAll + curStats.numRemoveAll;

        throughput[currentIteration] = ((double) curStats.total / elapsedTime);

        printStage("Benchmark statistics");

        int finalSize = dataStructure.size();
        curBenchStats.finalSize = finalSize;

//        System.out.println("  Final size:              \t" + finalSize);
        assert finalSize == (prefillSize + curStats.numAdd - curStats.numRemove) : "Final size does not reflect the modifications.";

        //System.out.println("  Otherupdate_bin.bat size:              \t" + map.size());

        // TODO what should print special for maint data structures
        // if (bench instanceof CASSpecFriendlyTreeLockFree) {
        // System.out.println("  Balance:              \t"
        // + ((CASSpecFriendlyTreeLockFree) bench).getBalance());
        // }
        // if (bench instanceof SpecFriendlyTreeLockFree) {
        // System.out.println("  Balance:              \t"
        // + ((SpecFriendlyTreeLockFree) bench).getBalance());
        // }
        // if (bench instanceof TransFriendlyMap) {
        // System.out.println("  Balance:              \t"
        // + ((TransFriendlyMap) bench).getBalance());
        // }
        // if (bench instanceof UpdatedTransFriendlySkipList) {
        // System.out.println("  Bottom changes:              \t"
        // + ((UpdatedTransFriendlySkipList) bench)
        // .getBottomLevelRaiseCount());
        // }


        if (dataStructure.getDataStructure() instanceof MaintenanceAlg) {
            curBenchStats.numNodes = ((MaintenanceAlg) dataStructure.getDataStructure()).numNodes();
        }

        System.out.println(curBenchStats);

        benchStatistics[currentIteration] = curBenchStats;
    }

    /**
     * Detailed Warmup TM Statistics
     */
    private int numCommits = 0;
    private int numStarts = 0;
    private int numAborts = 0;

    private int numCommitsReadOnly = 0;
    private int numCommitsElastic = 0;
    private int numCommitsUpdate = 0;

    private int numAbortsBetweenSuccessiveReads = 0;
    private int numAbortsBetweenReadAndWrite = 0;
    private int numAbortsExtendOnRead = 0;
    private int numAbortsWriteAfterRead = 0;
    private int numAbortsLockedOnWrite = 0;
    private int numAbortsLockedBeforeRead = 0;
    private int numAbortsLockedBeforeElasticRead = 0;
    private int numAbortsLockedOnRead = 0;
    private int numAbortsInvalidCommit = 0;
    private int numAbortsInvalidSnapshot = 0;

    private double readSetSizeSum = 0.0;
    private double writeSetSizeSum = 0.0;
    private int statSize = 0;
    private int txDurationSum = 0;
    private int elasticReads = 0;
    private int readsInROPrefix = 0;

    /**
     * This method is called between two runs of the benchmark within the same
     * JVM to enable its warmup
     */
    public void resetStats() {
        for (ThreadLoopAbstract threadLoop : threadLoops) {
            threadLoop.stats.reset();
        }
        this.curStats.reset();

        numCommits = 0;
        numStarts = 0;
        numAborts = 0;

        numCommitsReadOnly = 0;
        numCommitsElastic = 0;
        numCommitsUpdate = 0;

        numAbortsBetweenSuccessiveReads = 0;
        numAbortsBetweenReadAndWrite = 0;
        numAbortsExtendOnRead = 0;
        numAbortsWriteAfterRead = 0;
        numAbortsLockedOnWrite = 0;
        numAbortsLockedBeforeRead = 0;
        numAbortsLockedBeforeElasticRead = 0;
        numAbortsLockedOnRead = 0;
        numAbortsInvalidCommit = 0;
        numAbortsInvalidSnapshot = 0;

        readSetSizeSum = 0.0;
        writeSetSizeSum = 0.0;
        statSize = 0;
        txDurationSum = 0;
        elasticReads = 0;
        readsInROPrefix = 0;
    }

    public void recordPreliminaryStats() {
        numAborts = STMStatistics.getTotalAborts();
        numCommits = STMStatistics.getTotalCommits();
        numCommitsReadOnly = STMStatistics.getNumCommitsReadOnly();
        numCommitsElastic = STMStatistics.getNumCommitsElastic();
        numCommitsUpdate = STMStatistics.getNumCommitsUpdate();
        numStarts = STMStatistics.getTotalStarts();
        numAbortsBetweenSuccessiveReads = STMStatistics
                .getNumAbortsBetweenSuccessiveReads();
        numAbortsBetweenReadAndWrite = STMStatistics
                .getNumAbortsBetweenReadAndWrite();
        numAbortsExtendOnRead = STMStatistics.getNumAbortsExtendOnRead();
        numAbortsWriteAfterRead = STMStatistics.getNumAbortsWriteAfterRead();
        numAbortsLockedOnWrite = STMStatistics.getNumAbortsLockedOnWrite();
        numAbortsLockedBeforeRead = STMStatistics.getNumAbortsLockedBeforeRead();
        numAbortsLockedBeforeElasticRead = STMStatistics
                .getNumAbortsLockedBeforeElasticRead();
        numAbortsLockedOnRead = STMStatistics.getNumAbortsLockedOnRead();
        numAbortsInvalidCommit = STMStatistics.getNumAbortsInvalidCommit();
        numAbortsInvalidSnapshot = STMStatistics.getNumAbortsInvalidSnapshot();
        readSetSizeSum = STMStatistics.getSumReadSetSize();
        writeSetSizeSum = STMStatistics.getSumWriteSetSize();

        statSize = STMStatistics.getStatSize();
        txDurationSum = STMStatistics.getSumCommitingTxTime();
        elasticReads = STMStatistics.getTotalElasticReads();
        readsInROPrefix = STMStatistics.getTotalReadsInROPrefix();
    }

    /**
     * Print the detailed statistics on the standard output
     */
    private void printDetailedStats() {

        numCommits = STMStatistics.getTotalCommits() - numCommits;
        numStarts = STMStatistics.getTotalStarts() - numStarts;
        numAborts = STMStatistics.getTotalAborts() - numAborts;

        numCommitsReadOnly = STMStatistics.getNumCommitsReadOnly()
                - numCommitsReadOnly;
        numCommitsElastic = STMStatistics.getNumCommitsElastic()
                - numCommitsElastic;
        numCommitsUpdate = STMStatistics.getNumCommitsUpdate() - numCommitsUpdate;

        numAbortsBetweenSuccessiveReads = STMStatistics
                .getNumAbortsBetweenSuccessiveReads()
                - numAbortsBetweenSuccessiveReads;
        numAbortsBetweenReadAndWrite = STMStatistics
                .getNumAbortsBetweenReadAndWrite()
                - numAbortsBetweenReadAndWrite;
        numAbortsExtendOnRead = STMStatistics.getNumAbortsExtendOnRead()
                - numAbortsExtendOnRead;
        numAbortsWriteAfterRead = STMStatistics.getNumAbortsWriteAfterRead()
                - numAbortsWriteAfterRead;
        numAbortsLockedOnWrite = STMStatistics.getNumAbortsLockedOnWrite()
                - numAbortsLockedOnWrite;
        numAbortsLockedBeforeRead = STMStatistics.getNumAbortsLockedBeforeRead()
                - numAbortsLockedBeforeRead;
        numAbortsLockedBeforeElasticRead = STMStatistics
                .getNumAbortsLockedBeforeElasticRead()
                - numAbortsLockedBeforeElasticRead;
        numAbortsLockedOnRead = STMStatistics.getNumAbortsLockedOnRead()
                - numAbortsLockedOnRead;
        numAbortsInvalidCommit = STMStatistics.getNumAbortsInvalidCommit()
                - numAbortsInvalidCommit;
        numAbortsInvalidSnapshot = STMStatistics.getNumAbortsInvalidSnapshot()
                - numAbortsInvalidSnapshot;

        assert (numAborts == (numAbortsBetweenSuccessiveReads
                + numAbortsBetweenReadAndWrite + numAbortsExtendOnRead
                + numAbortsWriteAfterRead + numAbortsLockedOnWrite
                + numAbortsLockedBeforeRead + numAbortsLockedBeforeElasticRead
                + numAbortsLockedOnRead + numAbortsInvalidCommit + numAbortsInvalidSnapshot));

        assert (numStarts - numAborts) == numCommits;

        readSetSizeSum = STMStatistics.getSumReadSetSize() - readSetSizeSum;
        writeSetSizeSum = STMStatistics.getSumWriteSetSize() - writeSetSizeSum;
        statSize = STMStatistics.getStatSize() - statSize;
        txDurationSum = STMStatistics.getSumCommitingTxTime() - txDurationSum;

        printLine('-');
        System.out.println("TM statistics");
        printLine('-');

        System.out.println("  Commits:                  \t" + numCommits);
        System.out
                .println("  |--regular read only  (%) \t"
                        + numCommitsReadOnly
                        + "\t( "
                        + formatDouble(((double) numCommitsReadOnly / (double) numCommits) * 100)
                        + " %)");
        System.out
                .println("  |--elastic (%)            \t"
                        + numCommitsElastic
                        + "\t( "
                        + formatDouble(((double) numCommitsElastic / (double) numCommits) * 100)
                        + " %)");
        System.out
                .println("  |--regular update (%)     \t"
                        + numCommitsUpdate
                        + "\t( "
                        + formatDouble(((double) numCommitsUpdate / (double) numCommits) * 100)
                        + " %)");
        System.out.println("  Starts:                   \t" + numStarts);
        System.out.println("  Aborts:                   \t" + numAborts
                + "\t( 100 %)");
        System.out
                .println("  |--between succ. reads:   \t"
                        + (numAbortsBetweenSuccessiveReads)
                        + "\t( "
                        + formatDouble(((double) (numAbortsBetweenSuccessiveReads) * 100)
                        / (double) numAborts) + " %)");
        System.out
                .println("  |--between read & write:  \t"
                        + numAbortsBetweenReadAndWrite
                        + "\t( "
                        + formatDouble(((double) numAbortsBetweenReadAndWrite / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--extend upon read:      \t"
                        + numAbortsExtendOnRead
                        + "\t( "
                        + formatDouble(((double) numAbortsExtendOnRead / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--write after read:      \t"
                        + numAbortsWriteAfterRead
                        + "\t( "
                        + formatDouble(((double) numAbortsWriteAfterRead / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--locked on write:       \t"
                        + numAbortsLockedOnWrite
                        + "\t( "
                        + formatDouble(((double) numAbortsLockedOnWrite / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--locked before read:    \t"
                        + numAbortsLockedBeforeRead
                        + "\t( "
                        + formatDouble(((double) numAbortsLockedBeforeRead / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--locked before eread:   \t"
                        + numAbortsLockedBeforeElasticRead
                        + "\t( "
                        + formatDouble(((double) numAbortsLockedBeforeElasticRead / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--locked on read:        \t"
                        + numAbortsLockedOnRead
                        + "\t( "
                        + formatDouble(((double) numAbortsLockedOnRead / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--invalid commit:        \t"
                        + numAbortsInvalidCommit
                        + "\t( "
                        + formatDouble(((double) numAbortsInvalidCommit / (double) numAborts) * 100)
                        + " %)");
        System.out
                .println("  |--invalid snapshot:      \t"
                        + numAbortsInvalidSnapshot
                        + "\t( "
                        + formatDouble(((double) numAbortsInvalidSnapshot / (double) numAborts) * 100)
                        + " %)");
        System.out.println("  Read set size on avg.:    \t"
                + formatDouble(readSetSizeSum / statSize));
        System.out.println("  Write set size on avg.:   \t"
                + formatDouble(writeSetSizeSum / statSize));
        System.out.println("  Tx time-to-commit on avg.:\t"
                + formatDouble((double) txDurationSum / numCommits)
                + " microsec");
        System.out.println("  Number of elastic reads       " + elasticReads);
        System.out
                .println("  Number of reads in RO prefix  " + readsInROPrefix);
    }

    /**
     * Print the iteration statistics on the standard output
     */
    private void printIterationStats() {
        printLine('-');
        System.out.println("Iteration statistics");
        printLine('-');

        int n = parameters.iterations;
        System.out.println("  Iterations:                 \t" + n);
        double sum = 0;
        for (int i = 0; i < n; i++) {
            sum += ((throughput[i] / 1024) / 1024);
        }
        System.out.println("  Total throughput (mebiops/s): " + sum);
        double mean = sum / n;
        System.out.println("  |--Mean:                    \t" + mean);
        double temp = 0;
        for (int i = 0; i < n; i++) {
            double diff = ((throughput[i] / 1024) / 1024) - mean;
            temp += diff * diff;
        }
        double var = temp / n;
        System.out.println("  |--Variance:                \t" + var);
        double stdevp = java.lang.Math.sqrt(var);
        System.out.println("  |--Standard deviation pop:  \t" + stdevp);
        double sterr = stdevp / java.lang.Math.sqrt(n);
        System.out.println("  |--Standard error:          \t" + sterr);
        System.out.println("  |--Margin of error (95% CL):\t" + (sterr * 1.96));
    }


}
