package contention.benchmark.keygenerators.parameters;

import contention.abstractions.Parameters;

public class SkewedSetsParameters extends Parameters {
    public SkewedSetParameters READ = new SkewedSetParameters(0, 0);
    public SkewedSetParameters WRITE = new SkewedSetParameters(0, 0);
    public double INTERSECTION = 0;

    @Override
    public void parse(String[] args) {
        argNumber = 1;

        while (argNumber < args.length) {
            //todo
            super.parse(args);
        }
    }

    @Override
    public StringBuilder toStringBuilder() {
        //todo
        return super.toStringBuilder();
    }
}
