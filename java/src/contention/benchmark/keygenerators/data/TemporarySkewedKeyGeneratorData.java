package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.benchmark.Parameters;
import contention.benchmark.keygenerators.parameters.TemporarySkewedParameters;

public class TemporarySkewedKeyGeneratorData extends KeyGeneratorData {
    private final int[] setsLengths;
    private final int[] setsBegins;

    public TemporarySkewedKeyGeneratorData() {
        super();

        setsLengths = new int[TemporarySkewedParameters.setCount + 1];
        setsBegins = new int[TemporarySkewedParameters.setCount + 1];
        setsBegins[0] = 0;

        for (int i = 0; i < TemporarySkewedParameters.setCount; i++) {
            setsLengths[i] = (int) (Parameters.range * TemporarySkewedParameters.setSizes[i]);
            setsBegins[i + 1] = setsBegins[i] + setsLengths[i];
        }
    }
}
