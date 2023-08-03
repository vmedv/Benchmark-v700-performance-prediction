package contention.benchmark.workload.stop.condition;

import contention.benchmark.workload.Parameters;

public interface StopCondition {

    void start(Parameters parameters);

    boolean isStopped(int id);

    StringBuilder toStringBuilder();
}
