package contention.benchmark.keygenerators.abstractions;

public interface KeyGenerator {
    int nextGet();

    int nextInsert();

    int nextRemove();

    int nextPrefill();

}
