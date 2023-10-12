package contention.benchmark.workload.thread.loops.builders;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.args.generators.abstractions.ArgsGeneratorBuilder;
import contention.benchmark.workload.args.generators.builders.DefaultArgsGeneratorBuilder;
import contention.benchmark.workload.stop.condition.StopCondition;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.thread.loops.impls.TemporaryOperationsThreadLoop;
import contention.benchmark.workload.thread.loops.parameters.RatioThreadLoopParameters;

import java.lang.reflect.Method;

import static contention.benchmark.tools.StringFormat.indentedTitle;
import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

public class TemporaryOperationsThreadLoopBuilder extends ThreadLoopBuilder {
    public int stagesNumber = 0;
    public int[] stagesDurations;
    public RatioThreadLoopParameters[] ratios;

    public ArgsGeneratorBuilder argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();

    public TemporaryOperationsThreadLoopBuilder setStagesNumber(final int stagesNumber) {
        this.stagesNumber = stagesNumber;
        stagesDurations = new int[stagesNumber];
        ratios = new RatioThreadLoopParameters[stagesNumber];
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setStagesDurations(int[] stagesDurations) {
        this.stagesDurations = stagesDurations;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setStageDuration(int index, int stagesDuration) {
        assert (index < stagesNumber);

        stagesDurations[index] = stagesDuration;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setRatios(RatioThreadLoopParameters[] ratios) {
        this.ratios = ratios;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setRatios(int index, RatioThreadLoopParameters ratio) {
        assert (index < stagesNumber);

        ratios[index] = ratio;
        return this;
    }

    public TemporaryOperationsThreadLoopBuilder setArgsGeneratorBuilder(ArgsGeneratorBuilder argsGeneratorBuilder) {
        this.argsGeneratorBuilder = argsGeneratorBuilder;
        return this;
    }

    @Override
    public TemporaryOperationsThreadLoopBuilder init(int range) {
        argsGeneratorBuilder.init(range);
        return this;
    }

    @Override
    public TemporaryOperationsThreadLoop build(int threadId, DataStructure<Integer> dataStructure,
                                               Method[] methods, StopCondition stopCondition) {
        return new TemporaryOperationsThreadLoop(threadId, dataStructure, methods, stopCondition,
                stagesNumber, stagesDurations, ratios, argsGeneratorBuilder.build());
    }

    @Override
    public StringBuilder toStringBuilder(int indents) {
        StringBuilder result = new StringBuilder()
                .append(indentedTitleWithData("Type", "TEMPORARY_OPERATION", indents))
                .append(indentedTitleWithData("Stages Number", stagesNumber, indents))
                .append(indentedTitle("Stages Durations", indents));

        for (int i = 0; i < stagesNumber; i++) {
            result.append(indentedTitleWithData("Stage Duration " + i, stagesDurations[i], indents + 1));
        }

        result.append(indentedTitle("Ratios", indents));

        for (int i = 0; i < stagesNumber; i++) {
            result.append(indentedTitle("Ratio " + i, indents + 1))
                    .append(ratios[i].toStringBuilder(indents + 2));
        }

        result.append(indentedTitle("ArgsGenerator", indents))
                .append(argsGeneratorBuilder.toStringBuilder(indents + 1));

        return result;
    }
}
