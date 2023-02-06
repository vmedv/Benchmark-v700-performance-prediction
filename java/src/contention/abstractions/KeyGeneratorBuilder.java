package contention.abstractions;

public abstract class KeyGeneratorBuilder {
//    public KeyGeneratorType keyGeneratorType;
    public Parameters parameters;

    public KeyGeneratorBuilder(Parameters parameters) {
        this.parameters = parameters;
    }

    public abstract KeyGenerator[] generateKeyGenerators();

}
