package contention.benchmark.json.serializers;

import contention.benchmark.workload.stop.condition.StopCondition;

public class StopConditionSerializer extends BuilderSerializer<StopCondition> {

    @Override
    protected String getClassPath() {
        return "contention.benchmark.workload.stop.condition.";
    }
}
