package contention.benchmark.workload.args.generators.abstractions;

public interface ArgsGeneratorBuilder {
    ArgsGeneratorBuilder init(int range);

    ArgsGenerator build();

    default StringBuilder toStringBuilder() {
        return toStringBuilder(1);
    }

    StringBuilder toStringBuilder(int indents);
}
