package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.keygenerators.data.SkewedSetsKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SkewedSetsParameters;

public class SkewedSetsKeyGenerator implements KeyGenerator {
    private static SkewedSetsKeyGeneratorData data;
    private static SkewedSetsParameters parameters;

    private final Distribution readDistribution;
    private final Distribution writeDistribution;

    public SkewedSetsKeyGenerator(Distribution readDistribution, Distribution writeDistribution) {
        this.readDistribution = readDistribution;
        this.writeDistribution = writeDistribution;
    }

    public static void setData(SkewedSetsKeyGeneratorData data) {
        SkewedSetsKeyGenerator.data = data;
    }

    @Override
    public int nextRead() {
        return 0;
    }

    @Override
    public int nextInsert() {
        return 0;
    }

    @Override
    public int nextErase() {
        return 0;
    }

    @Override
    public int nextPrefill() {
        return 0;
    }

    public static KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        parameters = (SkewedSetsParameters) rawParameters;

        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        SkewedSetsKeyGeneratorData data = new SkewedSetsKeyGeneratorData(parameters);

        SkewedSetsKeyGenerator.setData(data);

        for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
            keygens[threadNum] = new SkewedSetsKeyGenerator(
                    new SkewedSetsDistribution(
                            data.readHotLength,
                            parameters.READ.HOT_PROB,
                            new UniformDistribution(data.readHotLength),
                            new UniformDistribution(parameters.range - data.readHotLength)
                    ),
                    new SkewedSetsDistribution(
                            data.writeHotLength,
                            parameters.WRITE.HOT_PROB,
                            new UniformDistribution(data.writeHotLength),
                            new UniformDistribution(parameters.range - data.writeHotLength)
                    )
            );
        }

        return keygens;
    }
}
