package contention.benchmark.distributions.abstractions;

import contention.abstractions.ParseArgument;
import contention.benchmark.distributions.SkewedUniformDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.distributions.ZipfDistribution;
import contention.benchmark.distributions.parameters.*;

public class DistributionBuilder {
    public DistributionType type;
    public DistributionParameters parameters;

    public DistributionBuilder(DistributionType distributionType) {
        this.type = distributionType;
    }

    public DistributionBuilder() {
        this.type = DistributionType.UNIFORM;
    }

    public DistributionBuilder setType(DistributionType type) {
        this.type = type;
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
            case "-dist-zipf" -> this.setType(DistributionType.ZIPF)
                    .setParameters(new ZipfParameters(Double.parseDouble(args.getNext())));
            case "-dist-skewed-uniform" -> this.setType(DistributionType.SKEWED_UNIFORM)
                    .setParameters(new SkewedUniformParameters(
                            Double.parseDouble(args.getNext()),
                            Double.parseDouble(args.getNext()))
                    );
            case "-dist-uniform" -> this.setType(DistributionType.UNIFORM);
            default -> {
                return false;
            }
        }
        return true;
    }

    public Distribution build(int range) {
        return switch (this.type) {
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
                        skewedUniformParameters.hotDistBuilder.build(skewedUniformParameters.getHotLength(range)),
                        skewedUniformParameters.coldDistBuilder.build(skewedUniformParameters.getColdLength(range))
                );
            }
        };
    }

    public MutableDistribution build() {
        return switch (this.type) {
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
