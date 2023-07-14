package contention.benchmark.ThreadLoops.abstractions;

import contention.abstractions.DataStructure;
import contention.benchmark.Parameters;
import contention.abstractions.ParseArgument;
import contention.benchmark.ThreadLoops.impls.*;
import contention.benchmark.ThreadLoops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.ThreadLoops.parameters.DeleteLeafsParameters;
import contention.benchmark.ThreadLoops.parameters.TemporaryOperationsThreadLoopParameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.stop.condition.StopCondition;

import java.lang.reflect.Method;

public class ThreadLoopBuilder {
    public ThreadLoopType type;
    public ThreadLoopParameters parameters;
    public int quantity;
    private StopCondition stopCondition;


    public ThreadLoopBuilder() {
        this.type = ThreadLoopType.DEFAULT;
        this.parameters = new DefaultThreadLoopParameters();
        this.quantity = 1;
    }

    public ThreadLoopBuilder(ThreadLoopType type) {
        this.type = type;
//        this.parameters = parameters;
    }

    public ThreadLoopBuilder(ThreadLoopParameters parameters) {
//        this.type = parameters.type;
        this.parameters = parameters;
    }

    public ThreadLoopBuilder setType(ThreadLoopType type) {
        this.type = type;
        return this;
    }

    public ThreadLoopBuilder setParameters(ThreadLoopParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public ThreadLoopBuilder setQuantity(int quantity) {
        this.quantity = quantity;
        return this;
    }

    public void init(Parameters generalParameters) {
        stopCondition = generalParameters.stopCondition;
        parameters.init(generalParameters);
    }

    public StringBuilder toStringBuilder() {
        return new StringBuilder()
                .append("  Thread Loop:             \t")
                .append(type)
                .append("\n")
                .append("  Quantity:                \t")
                .append(quantity)
                .append("\n")
                .append(parameters.toStringBuilder());
    }

    public ThreadLoop build(int threadNum, DataStructure<Integer> dataStructure, Method[] methods) {
        return switch (type) {
            case DEFAULT ->
                    new DefaultThreadLoop(threadNum, dataStructure, methods, stopCondition,
                            (DefaultThreadLoopParameters) parameters);
            case DELETE_SPEED_TEST -> new DeleteSpeedTest(threadNum, dataStructure, methods);
            case DELETE_LEAFS ->
                    new DeleteLeafsWorkload(threadNum, dataStructure, methods, stopCondition,
                            (DeleteLeafsParameters) parameters);
            case TEMPORARY_OPERATIONS ->
                    new TemporaryOperationsThreadLoop(threadNum, dataStructure, methods, stopCondition,
                            (TemporaryOperationsThreadLoopParameters) parameters);
            case TEMPORARY_OPERATIONS_2 ->
                    new TemporaryOperations2ThreadLoop(threadNum, dataStructure, methods, stopCondition,
                            (TemporaryOperationsThreadLoopParameters) parameters);
        };
    }

    public ThreadLoopBuilder parseThreadLoop(ParseArgument args) {
        return switch (args.getCurrent()) {
            case "-delete-leafs" -> {
                args.next();
                yield this.setType(ThreadLoopType.DELETE_LEAFS).setParameters(new DeleteLeafsParameters());
            }
            case "-temp-oper", "-temporary-operation" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
                parameters.keyGeneratorBuilder = KeyGeneratorBuilder.parseKeyGenerator(args);
                yield this.setType(ThreadLoopType.TEMPORARY_OPERATIONS).setParameters(parameters);
            }
            case "-temp-oper-2", "-temporary-operation-2" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
                parameters.keyGeneratorBuilder = KeyGeneratorBuilder.parseKeyGenerator(args);
                yield this.setType(ThreadLoopType.TEMPORARY_OPERATIONS_2).setParameters(parameters);
            }
            default -> {
                DefaultThreadLoopParameters parameters = new DefaultThreadLoopParameters();
                parameters.keyGeneratorBuilder = KeyGeneratorBuilder.parseKeyGenerator(args);
                yield this.setType(ThreadLoopType.DEFAULT).setParameters(parameters);
            }
        };
    }


    public static ThreadLoopBuilder parseAndCreateThreadLoop(ParseArgument args) {
        return switch (args.getCurrent()) {
            case "-delete-leafs" -> {
                args.next();
                yield new ThreadLoopBuilder(ThreadLoopType.DELETE_LEAFS);
            }
            case "-temp-oper", "-temporary-operation" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
                parameters.keyGeneratorBuilder = KeyGeneratorBuilder.parseKeyGenerator(args);
                yield new ThreadLoopBuilder(ThreadLoopType.TEMPORARY_OPERATIONS).setParameters(parameters);
            }
            case "-temp-oper-2", "-temporary-operation-2" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
                parameters.keyGeneratorBuilder = KeyGeneratorBuilder.parseKeyGenerator(args);
                yield new ThreadLoopBuilder(ThreadLoopType.TEMPORARY_OPERATIONS_2).setParameters(parameters);
            }
            default -> {
                DefaultThreadLoopParameters parameters = new DefaultThreadLoopParameters();
                parameters.keyGeneratorBuilder = KeyGeneratorBuilder.parseKeyGenerator(args);
                yield new ThreadLoopBuilder().setParameters(parameters);
            }
        };
    }
}
