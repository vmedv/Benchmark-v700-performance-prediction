package contention.benchmark.workload;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoop;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.stop.condition.StopCondition;
import contention.benchmark.workload.stop.condition.Timer;

import java.lang.reflect.Method;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

import static contention.benchmark.tools.StringFormat.indentedTitle;
import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

/**
 * Parameters of the Java version of the
 * Synchrobench benchmark.
 */
public class Parameters {

    public int numThreads = 0;
    /**
     * a maxAwaitTime of 0 means to wait forever
     */
    public long maxAwaitTime = 0;

    public StopCondition stopCondition = new Timer(5000);

    public List<ThreadLoopSettings> threadLoopBuilders = new ArrayList<>();

    public Parameters setMaxAwaitTime(long maxAwaitTime) {
        this.maxAwaitTime = maxAwaitTime;
        return this;
    }

    public Parameters setStopCondition(StopCondition stopCondition) {
        this.stopCondition = stopCondition;
        return this;
    }

    public Parameters setThreadLoopBuilders(List<ThreadLoopSettings> threadLoopBuilders) {
        this.threadLoopBuilders = threadLoopBuilders;
        numThreads = 0;
        threadLoopBuilders.forEach(it -> numThreads += it.quantity);
        return this;
    }

    public Parameters addThreadLoopBuilder(ThreadLoopSettings threadLoopSettings) {
        this.threadLoopBuilders.add(threadLoopSettings);
        numThreads += threadLoopSettings.quantity;
        return this;
    }

    public Parameters addThreadLoopBuilder(ThreadLoopBuilder threadLoopBuilder, int quantity) {
        return addThreadLoopBuilder(new ThreadLoopSettings(threadLoopBuilder, quantity));
    }

    public Parameters addThreadLoopBuilder(ThreadLoopBuilder threadLoopBuilder) {
        return addThreadLoopBuilder(threadLoopBuilder, 1);
    }

    public StringBuilder toStringBuilder() {
        return toStringBuilder(1);
    }

    public StringBuilder toStringBuilder(int indents) {
        StringBuilder params = new StringBuilder()
                .append(indentedTitleWithData("Number of threads", numThreads, indents))
                .append(indentedTitle("Stop condition", indents))
                .append(stopCondition.toStringBuilder(indents + 1))
                .append(indentedTitleWithData("Number of workloads", threadLoopBuilders.size(), indents))
                .append(indentedTitle("Thread loops", indents));

        threadLoopBuilders.forEach(it -> params.append(it.threadLoopBuilder.toStringBuilder(indents + 1)).append("\n"));
        return params;
    }

    public void init(int range) {
        AtomicInteger finalNumThreads = new AtomicInteger();
        threadLoopBuilders.forEach(it -> {
            it.threadLoopBuilder.init(range);
            finalNumThreads.addAndGet(it.quantity);
        });
        if (finalNumThreads.get() != numThreads) {
            System.err.println("WARNING: the specified number of threads ("
                    + numThreads + ") differs from the final one (" + finalNumThreads.get() + ")."
                    + "Last number of threads (" + finalNumThreads.get() + ") will be used .");
            numThreads = finalNumThreads.get();
        }
    }

    public ThreadLoop[] getWorkload(DataStructure<Integer> dataStructure, Method[] methods) {
        ThreadLoop[] workload = new ThreadLoop[numThreads];
        for (int threadNum = 0, i = 0, curQuantity = 0; threadNum < numThreads; ++threadNum, ++curQuantity) {
            if (curQuantity >= threadLoopBuilders.get(i).quantity) {
                curQuantity = 0;
                ++i;
            }

            workload[threadNum] = threadLoopBuilders.get(i)
                    .threadLoopBuilder.build(threadNum, dataStructure, methods, stopCondition);
        }

        return workload;
    }

    public static class ThreadLoopSettings {
        int quantity;
        public ThreadLoopBuilder threadLoopBuilder;

        public ThreadLoopSettings() {
        }

        public ThreadLoopSettings(ThreadLoopBuilder threadLoopBuilder, int quantity) {
            this.quantity = quantity;
            this.threadLoopBuilder = threadLoopBuilder;
        }
    }

}
