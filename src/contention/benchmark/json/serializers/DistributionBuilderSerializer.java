package contention.benchmark.json.serializers;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;

public class DistributionBuilderSerializer extends BuilderSerializer<DistributionBuilder> {

    @Override
    protected String getClassPath() {
        return "contention.benchmark.workload.distributions.builders.";
    }
}
