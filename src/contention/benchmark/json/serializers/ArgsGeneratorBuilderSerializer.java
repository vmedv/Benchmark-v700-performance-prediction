package contention.benchmark.json.serializers;

import contention.benchmark.workload.args.generators.abstractions.ArgsGeneratorBuilder;

public class ArgsGeneratorBuilderSerializer extends BuilderSerializer<ArgsGeneratorBuilder> {

    @Override
    protected String getClassPath() {
        return "contention.benchmark.workload.args.generators.builders.";
    }
}
