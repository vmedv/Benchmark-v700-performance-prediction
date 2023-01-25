package contention.benchmark.keygenerators;

import contention.abstractions.Distribution;
import contention.abstractions.KeyGenerator;
import contention.abstractions.Parameters;
import contention.benchmark.distributions.SkewedSetsDistribution;
import contention.benchmark.distributions.UniformDistribution;
import contention.benchmark.distributions.ZipfDistribution;
import contention.benchmark.keygenerators.data.SimpleKeyGeneratorData;
import contention.benchmark.keygenerators.parameters.SimpleParameters;

public class SimpleKeyGenerator implements KeyGenerator {
    private static SimpleKeyGeneratorData data = null;
    private static SimpleParameters parameters;

    private final Distribution distribution;

    public SimpleKeyGenerator(Distribution distribution) {
        this.distribution = distribution;
    }

    public static void setData(SimpleKeyGeneratorData data) {
        SimpleKeyGenerator.data = data;
    }

    private int next() {
        int value = distribution.next();
        return data == null ? value : data.get(value);
    }

    @Override
    public int nextRead() {
        return next();
    }

    @Override
    public int nextInsert() {
        return next();
    }

    @Override
    public int nextErase() {
        return next();
    }

    @Override
    public int nextPrefill() {
        return next();
    }

    public static KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        parameters = (SimpleParameters) rawParameters;
        KeyGenerator[] keygens = new KeyGenerator[parameters.numThreads];

        switch (parameters.distributionType) {
            case UNIFORM:
                for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
                    keygens[threadNum] = new SimpleKeyGenerator(new UniformDistribution(parameters.range));
                }
                break;
            case ZIPF:
            case MUTABLE_ZIPF:
                SimpleKeyGenerator.setData(new SimpleKeyGeneratorData(parameters));

                for (short threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
                    keygens[threadNum] = new SimpleKeyGenerator(
                            new ZipfDistribution(parameters.zipfParm, parameters.range)
                    );
                }
                break;
            case SKEWED_SETS:
                SimpleKeyGenerator.setData(new SimpleKeyGeneratorData(parameters));

                int hotLength = (int) (parameters.range * parameters.skewedSetParameters.HOT_SIZE);
                for (int threadNum = 0; threadNum < parameters.numThreads; threadNum++) {
                    keygens[threadNum] = new SimpleKeyGenerator(
                            new SkewedSetsDistribution(
                                    hotLength,
                                    parameters.skewedSetParameters.HOT_PROB,
                                    new UniformDistribution(hotLength),
                                    new UniformDistribution(parameters.range - hotLength)
                            )
                    );
                }
                break;
        }

        return keygens;
    }
}
