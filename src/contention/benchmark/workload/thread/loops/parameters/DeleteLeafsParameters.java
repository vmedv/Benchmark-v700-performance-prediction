package contention.benchmark.workload.thread.loops.parameters;

import contention.abstractions.ParseArgument;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopParameters;

public class DeleteLeafsParameters {
    public int range;

    public void init(int range) {
        this.range = range;
    }

    public boolean parseArg(ParseArgument args) {
        return false;
    }

    public StringBuilder toStringBuilder() {
        return new StringBuilder();
    }
}
