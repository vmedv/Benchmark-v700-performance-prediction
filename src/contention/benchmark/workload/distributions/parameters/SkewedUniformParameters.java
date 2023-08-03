package contention.benchmark.workload.distributions.parameters;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.builders.UniformDistributionBuilder;

public class SkewedUniformParameters  {
    public double HOT_SIZE = 0;
    public double HOT_PROB = 0;

    public DistributionBuilder hotDistBuilder = new UniformDistributionBuilder();
    public DistributionBuilder coldDistBuilder = new UniformDistributionBuilder();

    public SkewedUniformParameters() {
    }

    public SkewedUniformParameters(double HOT_SIZE, double HOT_PROB) {
        this.HOT_SIZE = HOT_SIZE;
        this.HOT_PROB = HOT_PROB;
    }

    public int getHotLength(int range) {
        return (int) (range * HOT_SIZE);
    }

    public int getColdLength(int range) {
        return range - this.getHotLength(range);
    }

    protected void parseArg() {
        //todo
//        super.parseArg();
    }

    public StringBuilder toStringBuilder() {
        //todo
        return new StringBuilder();
    }
}
