package contention.benchmark.workload.ThreadLoops.abstractions;

import contention.abstractions.ParseArgument;

public abstract class ThreadLoopParameters {
    public ThreadLoopType type;
//    public abstract void build();
    public abstract void init(int range);

    public abstract boolean parseArg(ParseArgument args);

    public abstract StringBuilder toStringBuilder();
}
