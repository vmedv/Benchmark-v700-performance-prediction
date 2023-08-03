package contention.benchmark.workload.stop.condition;

import contention.benchmark.workload.Parameters;

public class OperationCounter implements StopCondition {
    private Counter[] counters;

//    private AtomicInteger barrier;


    /**
     * the limit for each thread
     */
    public long commonOperationLimit;

    public OperationCounter() {
    }


    public OperationCounter(long commonOperationLimit) {
        this.commonOperationLimit = commonOperationLimit;
    }

    @Override
    public void start(Parameters parameters) {
        long operationLimit = commonOperationLimit / parameters.numThreads;
        long remainder = commonOperationLimit % parameters.numThreads;

//        barrier = new AtomicInteger(parameters.numThreads);
        counters = new Counter[parameters.numThreads];

        for (int i = 0; i < parameters.numThreads; i++) {
            counters[i] = new Counter(operationLimit + (--remainder >= 0 ? 1 : 0));
        }
    }

    @Override
    public boolean isStopped(int id) {
        return counters[id].stop();
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder();
    }

    private class Counter {
        long operCount;

        public Counter(long operCount) {
            this.operCount = operCount;
        }

        public boolean stop() {
//            operCount++;
//            if (operCount >= operationLimit) {
//                barrier.incrementAndGet();
//                return true;
//            }
//            return false;

            return --operCount < 0;
        }
    }
}
