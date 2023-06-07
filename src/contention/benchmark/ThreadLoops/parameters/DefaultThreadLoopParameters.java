package contention.benchmark.ThreadLoops.parameters;

import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopParameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;

public class DefaultThreadLoopParameters extends ThreadLoopParameters {
    public double
            numWrites = 0.4,
            numInsert = 0.2,
            numRemove = 0.2,
            numWriteAlls = 0,
            numSnapshots = 0;

    public KeyGeneratorBuilder keyGeneratorBuilder;

    @Override
    public void build(Parameters parameters) {
        this.numWrites = this.numInsert + this.numRemove;
        keyGeneratorBuilder.build(parameters);
    }

    public boolean parseArg(ParseArgument args) {
        String currentArg = args.getCurrent();

        try {
            switch (currentArg) {
                case "--updates", "-u" -> {
                    this.numWrites = Double.parseDouble(args.getNext());
                    this.numInsert = this.numWrites / 2;
                    this.numRemove = this.numWrites / 2;
                }
                case "--insert", "-ui" -> this.numInsert = Double.parseDouble(args.getNext());
                case "--remove", "-ue" -> this.numRemove = Double.parseDouble(args.getNext());
                case "--writeAll", "-a" -> this.numWriteAlls = Double.parseDouble(args.getNext());
                case "--snapshots", "-s" -> this.numSnapshots = Double.parseDouble(args.getNext());
                default -> {
                    return keyGeneratorBuilder.parameters.parseArg(args);
                }
            }
        } catch (NumberFormatException e) {
            System.err.println("Number expected after option:  "
                    + currentArg + ". Ignoring...");
        }
        return true;
    }

    @Override
    public StringBuilder toStringBuilder() {
        return new StringBuilder()
                .append("  Write ratio:             \t")
                .append(this.numWrites)
                .append(" \n")
                .append("  Insert ratio:            \t")
                .append(this.numInsert)
                .append(" \n")
                .append("  Remove ratio:            \t")
                .append(this.numRemove)
                .append(" \n")
                .append("  WriteAll ratio:          \t")
                .append(this.numWriteAlls)
                .append(" \n")
                .append("  Snapshot ratio:          \t")
                .append(this.numSnapshots)
                .append(" \n")
                .append(keyGeneratorBuilder.parameters.toStringBuilder());
    }
}

