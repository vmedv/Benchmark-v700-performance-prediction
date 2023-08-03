package contention.benchmark.workload.key.generators.abstractions;

public interface KeyGenerator {
    int nextGet();

    int nextInsert();

    int nextRemove();
}
