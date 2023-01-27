package contention.benchmark.keygenerators.parameters;

import contention.abstractions.DistributionType;
import contention.abstractions.Parameters;

public class SimpleParameters extends Parameters {
    public DistributionType distributionType = DistributionType.UNIFORM;
    public double zipfParm;
    public SkewedSetParameters skewedSetParameters = new SkewedSetParameters(0, 0);

    @Override
    public void parse(String[] args) {
        while (argNumber < args.length) {

            switch (args[argNumber]) {
                case "-dist-zipf" -> {
                    distributionType = DistributionType.ZIPF;
                    zipfParm = Double.parseDouble(args[++argNumber]);
                }
                case "-dist-uniform" -> distributionType = DistributionType.UNIFORM;
                default -> super.parse(args);
            }

            argNumber++;
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = super.toStringBuilder();
        params.append("\n")
                .append("  Distribution:            \t")
                .append(distributionType);
        switch (distributionType) {
            case ZIPF:
                params.append("\n")
                        .append("  Zipf Parm:               \t")
                        .append(zipfParm);
                break;
        }
        return params;
    }
}
