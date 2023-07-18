package contention.benchmark.workload.keygenerators.keygenerators.abstractions;

public interface KeyGeneratorBuilder {
    KeyGeneratorBuilder init(int range);

    KeyGenerator build();

    StringBuilder toStringBuilder();
}
