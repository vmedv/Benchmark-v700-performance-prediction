package contention.benchmark.workload.ThreadLoops.parameters;

import contention.abstractions.ParseArgument;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopParameters;

public class DeleteLeafsParameters extends ThreadLoopParameters {
    public int range;

    @Override
    public void init(int range) {
        this.range = range;
    }

    @Override
    public boolean parseArg(ParseArgument args) {
        return false;
    }

    @Override
    public StringBuilder toStringBuilder() {
        return new StringBuilder();
    }
}
