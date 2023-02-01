package contention.benchmark.distributions.parameters;

import contention.abstractions.DistributionBuilder;
import contention.abstractions.DistributionParameters;
import contention.abstractions.DistributionType;
import contention.abstractions.Parameters;

public class SkewedSetParameters extends Parameters implements DistributionParameters {
    public double HOT_SIZE = 0;
    public double HOT_PROB = 0;

    public DistributionBuilder hotDistType = new DistributionBuilder();
    public DistributionBuilder coldDistType = new DistributionBuilder();

    public SkewedSetParameters(double HOT_SIZE, double HOT_PROB) {
        this.HOT_SIZE = HOT_SIZE;
        this.HOT_PROB = HOT_PROB;
    }

    public int getHotLength() {
        return (int) (range * HOT_SIZE);
    }

    public int getColdLength() {
        return range - this.getHotLength();
    }

    @Override
    protected void parseArg() {
        //todo
        super.parseArg();
    }

    @Override
    public StringBuilder toStringBuilder() {
        //todo
        return new StringBuilder();
    }
}
