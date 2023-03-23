package contention.abstractions;

public interface ThreadLoopParameters {
    void build();

    boolean parseArg(ParseArgument args);

    StringBuilder toStringBuilder();
}
