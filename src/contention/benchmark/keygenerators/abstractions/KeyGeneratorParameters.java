package contention.benchmark.keygenerators.abstractions;

import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;

public interface KeyGeneratorParameters {
    void init(Parameters parameters);

    boolean parseArg(ParseArgument args);

    StringBuilder toStringBuilder();

}
