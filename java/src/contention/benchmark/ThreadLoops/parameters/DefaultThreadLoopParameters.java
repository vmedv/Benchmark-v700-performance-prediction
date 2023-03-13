package contention.benchmark.ThreadLoops.parameters;

import contention.abstractions.ParseArgument;
import contention.abstractions.ThreadLoopParameters;

public class DefaultThreadLoopParameters implements ThreadLoopParameters {
    public double
            numWrites = 0.4,
            numInsert = 0.2,
            numErase = 0.2,
            numWriteAlls = 0,
            numSnapshots = 0;

    public void build() {
        this.numWrites = this.numInsert + this.numErase;
    }

    public boolean parseArg(ParseArgument args) {
        String currentArg = args.getCurrent();

        try {
            switch (currentArg) {
                case "--updates", "-u" -> {
                    this.numWrites = Double.parseDouble(args.getNext());
                    this.numInsert = this.numWrites / 2;
                    this.numErase = this.numWrites / 2;
                }
                case "--insert", "-ui" -> this.numInsert = Double.parseDouble(args.getNext());
                case "--erase", "-ue" -> this.numErase = Double.parseDouble(args.getNext());
                case "--writeAll", "-a" -> this.numWriteAlls = Double.parseDouble(args.getNext());
                case "--snapshots", "-s" -> this.numSnapshots = Double.parseDouble(args.getNext());
                default -> {
                    return true;
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
                .append("  Erase ratio:             \t")
                .append(this.numErase)
                .append(" \n")
                .append("  WriteAll ratio:          \t")
                .append(this.numWriteAlls)
                .append(" \n")
                .append("  Snapshot ratio:          \t")
                .append(this.numSnapshots)
                .append(" \n");
    }
}

