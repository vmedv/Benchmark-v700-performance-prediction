package contention.benchmark.workload.stop.condition;

import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

public class OperationCounter implements StopCondition {
    private Counter[] counters;

    /**
     * the limit for all thread
     */
    public long commonOperationLimit;

    public OperationCounter() {
    }

    public OperationCounter(long commonOperationLimit) {
        this.commonOperationLimit = commonOperationLimit;
    }

    public OperationCounter setCommonOperationLimit(long commonOperationLimit) {
        this.commonOperationLimit = commonOperationLimit;
        return this;
    }

    @Override
    public void start(int numThreads) {
        long operationLimit = commonOperationLimit / numThreads;
        long remainder = commonOperationLimit % numThreads;

        counters = new Counter[numThreads];

        for (int i = 0; i < numThreads; i++) {
            counters[i] = new Counter(operationLimit + (--remainder >= 0 ? 1 : 0));
        }
    }

    @Override
    public boolean isStopped(int id) {
        return counters[id].stop();
    }

    @Override
    public StringBuilder toStringBuilder(int indents) {
        return new StringBuilder()
                .append(indentedTitleWithData("Type", "OperationCounter", indents))
                .append(indentedTitleWithData("Common operation limit", commonOperationLimit, indents));
    }

    private static class Counter {
        long operCount;

        public Counter(long operCount) {
            this.operCount = operCount;
        }

        public boolean stop() {
            return --operCount < 0;
        }
    }
}
