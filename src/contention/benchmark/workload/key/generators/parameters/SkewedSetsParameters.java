package contention.benchmark.workload.key.generators.parameters;

import contention.abstractions.ParseArgument;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.data.map.builders.ArrayDataMapBuilder;
import contention.benchmark.workload.distributions.parameters.SkewedUniformParameters;

public class SkewedSetsParameters {
    public SkewedUniformParameters READ = new SkewedUniformParameters(0, 0);
    public SkewedUniformParameters WRITE = new SkewedUniformParameters(0, 0);

    public DataMapBuilder dataMapBuilder = new ArrayDataMapBuilder();

    public double INTERSECTION = 0;

    public int readHotLength;
    public int writeHotLength;

    public int writeHotBegin;
    public int writeHotEnd;

    public void init(int range) {
        int readHotLength = (int) (range * READ.HOT_SIZE);
        int writeHotLength = (int) (range * WRITE.HOT_SIZE);
        int intersectionLength = (int) (range * INTERSECTION);

        this.readHotLength = readHotLength;
        this.writeHotLength = writeHotLength;

        writeHotBegin = readHotLength - intersectionLength;
        writeHotEnd = writeHotBegin + writeHotLength;
    }

//    @Override
    public boolean parseArg(ParseArgument args) {
        //todo
//        super.parseArg(args);
        return false;
    }

//    @Override
    public StringBuilder toStringBuilder() {
        StringBuilder params = new StringBuilder();
        //todo
        return params;
    }
}
