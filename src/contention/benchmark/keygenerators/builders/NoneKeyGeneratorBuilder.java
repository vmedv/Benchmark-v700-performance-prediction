package contention.benchmark.keygenerators.builders;

import contention.abstractions.KeyGenerator;
import contention.abstractions.KeyGeneratorBuilder;
import contention.abstractions.Parameters;

public class NoneKeyGeneratorBuilder extends KeyGeneratorBuilder {

    public NoneKeyGeneratorBuilder(Parameters parameters) {
        super(parameters);
    }

    @Override
    public KeyGenerator[] generateKeyGenerators() {
        return null;
    }
}
