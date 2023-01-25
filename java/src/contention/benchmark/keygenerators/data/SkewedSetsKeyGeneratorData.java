package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGeneratorData extends KeyGeneratorData {
    public final int readHotLength;
    public final int writeHotLength;

    public final int writeHotBegin;
    public final int writeHotEnd;


    public SkewedSetsKeyGeneratorData(SkewedSetsParameters parameters) {
        super(parameters);

        int readHotLength = (int) (parameters.range * parameters.READ.HOT_SIZE);
        int writeHotLength = (int) (parameters.range * parameters.WRITE.HOT_SIZE);
        int intersectionLength = (int) (parameters.range * parameters.INTERSECTION);

        this.readHotLength = readHotLength;
        this.writeHotLength = writeHotLength;

        writeHotBegin = readHotLength - intersectionLength;
        writeHotEnd = writeHotBegin + writeHotLength;
    }
    //todo
}
