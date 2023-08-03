package contention.benchmark.workload.thread.loops.parameters;

import contention.abstractions.ParseArgument;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopParameters;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.key.generators.builders.DefaultKeyGeneratorBuilder;

public class DefaultThreadLoopParameters {
    public double
//            numWrites = 0.4,
            insertRatio = 0.2,
            removeRatio = 0.2,
            writeAllsRatio = 0,
            snapshotsRatio = 0;

//    public KeyGeneratorBuilder keyGeneratorBuilder = new DefaultKeyGeneratorBuilder();



    public boolean parseArg(ParseArgument args) {
        String currentArg = args.getCurrent();

        try {
            switch (currentArg) {
//                case "--updates", "-u" -> {
//                    this.numWrites = Double.parseDouble(args.getNext());
//                    this.insertRatio = this.numWrites / 2;
//                    this.removeRatio = this.numWrites / 2;
//                }
                case "--insert", "-ui" -> this.insertRatio = Double.parseDouble(args.getNext());
                case "--remove", "-ue" -> this.removeRatio = Double.parseDouble(args.getNext());
                case "--writeAll", "-a" -> this.writeAllsRatio = Double.parseDouble(args.getNext());
                case "--snapshots", "-s" -> this.snapshotsRatio = Double.parseDouble(args.getNext());
//                default -> {
//                    return keyGeneratorBuilder.parameters.parseArg(args);
//                }
            }
        } catch (NumberFormatException e) {
            System.err.println("Number expected after option:  "
                    + currentArg + ". Ignoring...");
        }
        return true;
    }

    public StringBuilder toStringBuilder() {
        return new StringBuilder()
//                .append("  Write ratio:             \t")
//                .append(this.numWrites)
//                .append(" \n")
                .append("  Insert ratio:            \t")
                .append(this.insertRatio)
                .append(" \n")
                .append("  Remove ratio:            \t")
                .append(this.removeRatio)
                .append(" \n")
                .append("  WriteAll ratio:          \t")
                .append(this.writeAllsRatio)
                .append(" \n")
                .append("  Snapshot ratio:          \t")
                .append(this.snapshotsRatio)
                .append(" \n");
//                .append(keyGeneratorBuilder.toStringBuilder());
    }
}

