package contention.abstractions;

public interface KeyGenerator {
    int nextRead();

    int nextInsert();

    int nextErase();

    int nextPrefill();

    static KeyGenerator[] generateKeyGenerators(Parameters rawParameters) {
        return null;
    }
}
