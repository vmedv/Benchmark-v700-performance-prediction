package contention.benchmark.keygenerators.parameters;

import contention.abstractions.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.distributions.parameters.SkewedUniformParameters;

public class SkewedSetsParameters extends Parameters {
    public SkewedUniformParameters READ = new SkewedUniformParameters(0, 0);
    public SkewedUniformParameters WRITE = new SkewedUniformParameters(0, 0);
    public double INTERSECTION = 0;


    public int readHotLength;
    public int writeHotLength;

    public int writeHotBegin;
    public int writeHotEnd;

    @Override
    public void build() {
        super.build();
        int readHotLength = (int) (range * READ.HOT_SIZE);
        int writeHotLength = (int) (range * WRITE.HOT_SIZE);
        int intersectionLength = (int) (range * INTERSECTION);

        this.readHotLength = readHotLength;
        this.writeHotLength = writeHotLength;

        writeHotBegin = readHotLength - intersectionLength;
        writeHotEnd = writeHotBegin + writeHotLength;
    }

    @Override
    protected void parseArg(ParseArgument args) {
        //todo
        super.parseArg(args);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //todo
        return super.toStringBuilder();
    }
}
