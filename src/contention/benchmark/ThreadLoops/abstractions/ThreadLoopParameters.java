package contention.benchmark.ThreadLoops.abstractions;

import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;

public abstract class ThreadLoopParameters {
    public ThreadLoopType type;
//    public abstract void build();
    public abstract void init(Parameters parameters);

    public abstract boolean parseArg(ParseArgument args);

    public abstract StringBuilder toStringBuilder();
}
