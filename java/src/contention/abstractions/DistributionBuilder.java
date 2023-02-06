package contention.abstractions;

import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.distributions.ZipfDistribution;
import contention.benchmark.distributions.parameters.*;

public class DistributionBuilder {
    public DistributionType distributionType;
    public DistributionParameters parameters;

    public DistributionBuilder(DistributionType distributionType) {
        this.distributionType = distributionType;
    }

    public DistributionBuilder() {
        this.distributionType = DistributionType.UNIFORM;
    }

    public DistributionBuilder setDistributionType(DistributionType distributionType) {
        this.distributionType = distributionType;
        return this;
    }

    public DistributionBuilder setParameters(DistributionParameters parameters) {
        this.parameters = parameters;
        return this;
    }

//    public DistributionBuilder parseDistribution(String[] args, int argNumber) {
//        return switch (args[argNumber]) {
//            case "-dist-zipf" -> this.setDistributionType(DistributionType.ZIPF)
//                    .setParameters(new ZipfParameters(Double.parseDouble(args[++argNumber])));
//            case "-dist-skewed-sets" ->
//                    this.setDistributionType(DistributionType.SKEWED_SETS); //todo add parameters parse
//            case "-dist-uniform" -> this.setDistributionType(DistributionType.UNIFORM);
//            default -> null;
//        };
//    }


    public int parseDistribution(String[] args, int argNumber) {
        switch (args[argNumber]) {
            case "-dist-zipf" -> this.setDistributionType(DistributionType.ZIPF)
                    .setParameters(new ZipfParameters(Double.parseDouble(args[++argNumber])));
            case "-dist-skewed-sets" ->
                    this.setDistributionType(DistributionType.SKEWED_SETS); //todo add parameters parse
            case "-dist-uniform" -> this.setDistributionType(DistributionType.UNIFORM);
        }
        return argNumber;
    }

    public Distribution getDistribution(int range) {
        return switch (this.distributionType) {
            case UNIFORM -> new UniformDistribution(range);
            case ZIPF -> {
                ZipfParameters zipfParameters = (ZipfParameters) parameters;
                yield new ZipfDistribution(zipfParameters.alpha, range);
            }
            case SKEWED_SETS -> {
                SkewedSetParameters skewedSetParameters = (SkewedSetParameters) parameters;
                yield new SkewedSetsDistribution(
                        skewedSetParameters.getHotLength(),
                        skewedSetParameters.HOT_PROB,
                        skewedSetParameters.hotDistType.getDistribution(skewedSetParameters.getHotLength()),
                        skewedSetParameters.coldDistType.getDistribution(skewedSetParameters.getColdLength())
                );
            }
        };
    }

    public MutableDistribution getDistribution() {
        return switch (this.distributionType) {
            case UNIFORM -> new UniformDistribution();
            case ZIPF -> {
                ZipfParameters zipfParameters = (ZipfParameters) parameters;
                yield new ZipfDistribution(zipfParameters.alpha);
            }
            default -> null;
        };
    }

    public StringBuilder toStringBuilderParameters() {
        return this.parameters != null ? this.parameters.toStringBuilder() : new StringBuilder();
    }

}
