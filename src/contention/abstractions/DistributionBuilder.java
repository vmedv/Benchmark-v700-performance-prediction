package contention.abstractions;

import contention.benchmark.distributions.SkewedUniformDistribution;
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

    /**
     * @return true if distribution is parsed
     */
    public boolean parseDistribution(ParseArgument args) {
        switch (args.getCurrent()) {
            case "-dist-zipf" -> this.setDistributionType(DistributionType.ZIPF)
                    .setParameters(new ZipfParameters(Double.parseDouble(args.getNext())));
            case "-dist-skewed-uniform" -> this.setDistributionType(DistributionType.SKEWED_UNIFORM)
                    .setParameters(new SkewedUniformParameters(
                            Double.parseDouble(args.getNext()),
                            Double.parseDouble(args.getNext()))
                    );
            case "-dist-uniform" -> this.setDistributionType(DistributionType.UNIFORM);
            default -> {
                return false;
            }
        }
        return true;
    }

    public Distribution getDistribution(int range) {
        return switch (this.distributionType) {
            case UNIFORM -> new UniformDistribution(range);
            case ZIPF -> {
                ZipfParameters zipfParameters = (ZipfParameters) parameters;
                yield new ZipfDistribution(zipfParameters.alpha, range);
            }
            case SKEWED_UNIFORM -> {
                SkewedUniformParameters skewedUniformParameters = (SkewedUniformParameters) parameters;
                yield new SkewedUniformDistribution(
                        skewedUniformParameters.getHotLength(range),
                        skewedUniformParameters.HOT_PROB,
                        skewedUniformParameters.hotDistBuilder.getDistribution(skewedUniformParameters.getHotLength(range)),
                        skewedUniformParameters.coldDistBuilder.getDistribution(skewedUniformParameters.getColdLength(range))
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
