package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;

public class SkewedSetsKeyGeneratorData extends KeyGeneratorData {
    private final int readHotLength;
    private final int writeHotLength;

    private final int writeHotBegin;
    private final int writeHotEnd;


    public SkewedSetsKeyGeneratorData(int readHotLength, int writeHotLength, int intersectionLength) {
        super();

        this.readHotLength = readHotLength;
        this.writeHotLength = writeHotLength;

        writeHotBegin = readHotLength - intersectionLength;
        writeHotEnd = writeHotBegin + writeHotLength;
    }
    //todo
}
