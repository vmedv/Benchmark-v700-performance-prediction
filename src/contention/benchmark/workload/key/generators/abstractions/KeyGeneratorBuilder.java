package contention.benchmark.workload.key.generators.abstractions;

public interface KeyGeneratorBuilder {
    KeyGeneratorBuilder init(int range);

    KeyGenerator build();

    StringBuilder toStringBuilder();
}
