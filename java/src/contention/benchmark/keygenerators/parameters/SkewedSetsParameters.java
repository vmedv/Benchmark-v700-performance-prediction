package contention.benchmark.keygenerators.parameters;

import contention.abstractions.Parameters;
import contention.benchmark.distributions.parameters.SkewedSetParameters;

public class SkewedSetsParameters extends Parameters {
    public SkewedSetParameters READ = new SkewedSetParameters(0, 0);
    public SkewedSetParameters WRITE = new SkewedSetParameters(0, 0);
    public double INTERSECTION = 0;

    @Override
    protected void parseArg() {
        //todo
        super.parseArg();
    }

    @Override
    public StringBuilder toStringBuilder() {
        //todo
        return super.toStringBuilder();
    }
}
