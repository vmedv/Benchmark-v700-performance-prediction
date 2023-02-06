package contention.benchmark.ThreadLoops.workloads.parameters;

import contention.benchmark.keygenerators.parameters.LeafsHandshakeParameters;

public class TemporaryOperationsParameters extends LeafsHandshakeParameters {
    public int tempOperCount = 0;
    public int[] opTimes;
    public int[] numInserts;
    public int[] numErases;


    public void setTempOperCount(final int tempOperCount) {
        this.tempOperCount = tempOperCount;
        opTimes = new int[tempOperCount];
        numErases = new int[tempOperCount];
        numInserts = new int[tempOperCount];
    }

    @Override
    protected void parseArg() {
        switch (args[argNumber]) {
            case "-temp-oper-count" -> setTempOperCount(Integer.parseInt(args[++argNumber]));
            case "-ot" -> opTimes[Integer.parseInt(args[++argNumber])] = Integer.parseInt(args[++argNumber]);
            case "-uii" -> numInserts[Integer.parseInt(args[++argNumber])] = Integer.parseInt(args[++argNumber]);
            case "-uei" -> numErases[Integer.parseInt(args[++argNumber])] = Integer.parseInt(args[++argNumber]);
            default -> super.parseArg();
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder result = super.toStringBuilder();
        result.append("\n")
                .append("  Number of temp Operations:\t")
                .append(this.tempOperCount)
                .append("\n")
                .append("  Writes ratios:           \t");

        for (int i = 0; i < tempOperCount; i++) {
            result
                .append("\n")
                .append("    Time of ").append(i).append(":             \t")
                .append(this.opTimes[i])
                .append(" op.\n")
                .append("    Write ratio of ").append(i).append(":      \t")
                .append(this.numInserts[i] + this.numErases[i])
                .append(" %\n")
                .append("    Insert ratio of ").append(i).append(":     \t")
                .append(this.numInserts[i])
                .append(" %\n")
                .append("    Erase ratio of ").append(i).append(":      \t")
                .append(this.numErases[i])
                .append(" %\n");
        }
        return result;
    }

}
