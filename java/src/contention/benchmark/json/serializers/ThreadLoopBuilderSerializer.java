package contention.benchmark.json.serializers;

import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;

public class ThreadLoopBuilderSerializer extends BuilderSerializer<ThreadLoopBuilder> {

    @Override
    protected String getClassPath() {
        return "contention.benchmark.workload.thread.loops.builders.";
    }
}
