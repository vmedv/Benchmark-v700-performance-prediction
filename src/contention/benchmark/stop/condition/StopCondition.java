package contention.benchmark.stop.condition;

import contention.benchmark.Parameters;

public interface StopCondition {

    void start(Parameters parameters);

    boolean isStopped(int id);

    StringBuilder toStringBuilder();
}
