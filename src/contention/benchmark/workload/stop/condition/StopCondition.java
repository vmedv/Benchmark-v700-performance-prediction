package contention.benchmark.workload.stop.condition;

public interface StopCondition {

    void start(int numThreads);

    boolean isStopped(int id);

    default StringBuilder toStringBuilder() {
        return toStringBuilder(1);
    }

    StringBuilder toStringBuilder(int indents);

}
