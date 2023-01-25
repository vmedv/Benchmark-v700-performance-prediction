package contention.benchmark.keygenerators.parameters;

import contention.abstractions.Parameters;

public class SkewedSetParameters extends Parameters {
    public double HOT_SIZE = 0;
    public double HOT_PROB = 0;

    public SkewedSetParameters(double HOT_SIZE, double HOT_PROB) {
        this.HOT_SIZE = HOT_SIZE;
        this.HOT_PROB = HOT_PROB;
    }

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
        return new StringBuilder();
    }
}
