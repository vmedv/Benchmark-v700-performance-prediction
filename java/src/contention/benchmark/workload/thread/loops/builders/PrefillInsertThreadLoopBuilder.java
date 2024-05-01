package contention.benchmark.workload.thread.loops.builders;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.args.generators.abstractions.ArgsGeneratorBuilder;
import contention.benchmark.workload.args.generators.builders.DefaultArgsGeneratorBuilder;
import contention.benchmark.workload.stop.condition.StopCondition;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.thread.loops.impls.PrefillInsertThreadLoop;

import java.lang.reflect.Method;

import static contention.benchmark.tools.StringFormat.indentedTitle;
import static contention.benchmark.tools.StringFormat.indentedTitleWithData;

public class PrefillInsertThreadLoopBuilder extends ThreadLoopBuilder {

    public ArgsGeneratorBuilder argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();
    int numberOfAttempts = 1_000_000;

    public PrefillInsertThreadLoopBuilder setArgsGeneratorBuilder(ArgsGeneratorBuilder argsGeneratorBuilder) {
        this.argsGeneratorBuilder = argsGeneratorBuilder;
        return this;
    }

    public PrefillInsertThreadLoopBuilder setNumberOfAttempts(int numberOfAttempts) {
        this.numberOfAttempts = numberOfAttempts;
        return this;
    }

    @Override
    public ThreadLoopBuilder init(int range) {
        argsGeneratorBuilder.init(range);
        return this;
    }

    @Override
    public PrefillInsertThreadLoop build(int threadId, DataStructure<Integer> dataStructure, Method[] methods, StopCondition stopCondition) {
        return new PrefillInsertThreadLoop(threadId, dataStructure, methods, stopCondition,
                argsGeneratorBuilder.build(), numberOfAttempts);
    }

    @Override
    public StringBuilder toStringBuilder(int indents) {
        return new StringBuilder()
                .append(indentedTitleWithData("Type", "Prefill Insert", indents))
                .append(indentedTitleWithData("Number of attempts", numberOfAttempts, indents))
                .append(indentedTitle("ArgsGenerator", indents))
                .append(argsGeneratorBuilder.toStringBuilder(indents + 1));
    }
}
