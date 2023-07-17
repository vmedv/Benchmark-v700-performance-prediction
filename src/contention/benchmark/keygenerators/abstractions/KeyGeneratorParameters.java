package contention.benchmark.keygenerators.abstractions;

import contention.abstractions.ParseArgument;

public interface KeyGeneratorParameters {
    void init(int range);

    boolean parseArg(ParseArgument args);

    StringBuilder toStringBuilder();

}
