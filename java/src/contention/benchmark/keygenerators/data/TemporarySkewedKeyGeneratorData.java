package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorData extends KeyGeneratorData {
    public final int[] setsLengths;
    public final int[] setsBegins;

    public TemporarySkewedKeyGeneratorData(TemporarySkewedParameters parameters) {
        super(parameters);

        setsLengths = new int[parameters.setCount + 1];
        setsBegins = new int[parameters.setCount + 1];
        setsBegins[0] = 0;

        for (int i = 0; i < parameters.setCount; i++) {
            setsLengths[i] = (int) (Parameters.range * parameters.setSizes[i]);
            setsBegins[i + 1] = setsBegins[i] + setsLengths[i];
        }
    }
}
