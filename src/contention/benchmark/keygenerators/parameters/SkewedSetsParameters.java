package contention.benchmark.keygenerators.parameters;

import contention.abstractions.ParseArgument;
import contention.benchmark.datamap.abstractions.DataMapBuilder;
import contention.benchmark.datamap.abstractions.DataMapType;
import contention.benchmark.distributions.parameters.SkewedUniformParameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;

public class SkewedSetsParameters implements KeyGeneratorParameters {
    public SkewedUniformParameters READ = new SkewedUniformParameters(0, 0);
    public SkewedUniformParameters WRITE = new SkewedUniformParameters(0, 0);

    public DataMapBuilder dataMapBuilder = new DataMapBuilder(DataMapType.ARRAY);

    public double INTERSECTION = 0;

    public int readHotLength;
    public int writeHotLength;

    public int writeHotBegin;
    public int writeHotEnd;

    @Override
    public void init(int range) {
        int readHotLength = (int) (range * READ.HOT_SIZE);
        int writeHotLength = (int) (range * WRITE.HOT_SIZE);
        int intersectionLength = (int) (range * INTERSECTION);

        this.readHotLength = readHotLength;
        this.writeHotLength = writeHotLength;

        writeHotBegin = readHotLength - intersectionLength;
        writeHotEnd = writeHotBegin + writeHotLength;
    }

    @Override
    public boolean parseArg(ParseArgument args) {
        //todo
//        super.parseArg(args);
        return false;
    }

    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = new StringBuilder();
        //todo
        return params;
    }
}
