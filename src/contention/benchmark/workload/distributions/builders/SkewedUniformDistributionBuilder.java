package contention.benchmark.workload.distributions.builders;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.SkewedUniformDistribution;
import contention.benchmark.workload.distributions.abstractions.Distribution;

public class SkewedUniformDistributionBuilder implements DistributionBuilder {
//    public int range;
    public double hotSize = 0;
    public double hotProb = 0;

    public DistributionBuilder hotDistBuilder = new UniformDistributionBuilder();
    public DistributionBuilder coldDistBuilder = new UniformDistributionBuilder();

//    public SkewedUniformDistributionBuilder setRange(int range) {
//        this.range = range;
//        return this;
//    }

    public SkewedUniformDistributionBuilder setHotSize(double hotSize) {
        this.hotSize = hotSize;
        return this;
    }

    public SkewedUniformDistributionBuilder setHotProb(double hotProb) {
        this.hotProb = hotProb;
        return this;
    }

//    public SkewedUniformDistributionBuilder setHotDistBuilder(DistributionBuilder hotDistBuilder) {
//        this.hotDistBuilder = hotDistBuilder;
//        return this;
//    }
//
//    public SkewedUniformDistributionBuilder setColdDistBuilder(DistributionBuilder coldDistBuilder) {
//        this.coldDistBuilder = coldDistBuilder;
//        return this;
//    }

    public int getHotLength(int range) {
        return (int) (range * hotSize);
    }

    public int getColdLength(int range) {
        return range - this.getHotLength(range);
    }

    @Override
    public Distribution build(int range) {
        return new SkewedUniformDistribution(
                getHotLength(range),
                hotProb,
                new UniformDistributionBuilder().build(getHotLength(range)),
                new UniformDistributionBuilder().build(getColdLength(range))
//                hotDistBuilder.build(skewedUniformParameters.getHotLength(range)),
//                coldDistBuilder.build(skewedUniformParameters.getColdLength(range))
        );
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO toStringBuilder
        return new StringBuilder();
    }
}
