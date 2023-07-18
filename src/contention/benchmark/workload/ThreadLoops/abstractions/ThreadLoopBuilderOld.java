package contention.benchmark.workload.ThreadLoops.abstractions;

import contention.abstractions.DataStructure;
import contention.abstractions.ParseArgument;
//import contention.benchmark.ThreadLoops.impls.*;
import contention.benchmark.workload.ThreadLoops.impls.*;
import contention.benchmark.workload.ThreadLoops.parameters.DefaultThreadLoopParameters;
import contention.benchmark.workload.ThreadLoops.parameters.DeleteLeafsParameters;
import contention.benchmark.workload.ThreadLoops.parameters.TemporaryOperationsThreadLoopParameters;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilderOld;
import contention.benchmark.stop.condition.StopCondition;
//import contention.workload.ThreadLoops.impls.*;

import java.lang.reflect.Method;

public class ThreadLoopBuilderOld {
    public ThreadLoopType type;
    public ThreadLoopParameters parameters;
    public int quantity;
    private StopCondition stopCondition;


    public ThreadLoopBuilderOld() {
        this.type = ThreadLoopType.DEFAULT;
        this.parameters = new DefaultThreadLoopParameters();
        this.quantity = 1;
    }

    public ThreadLoopBuilderOld(ThreadLoopType type) {
        this.type = type;
//        this.parameters = parameters;
    }

    public ThreadLoopBuilderOld(ThreadLoopParameters parameters) {
//        this.type = parameters.type;
        this.parameters = parameters;
    }

    public ThreadLoopBuilderOld setType(ThreadLoopType type) {
        this.type = type;
        return this;
    }

    public ThreadLoopBuilderOld setParameters(ThreadLoopParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public ThreadLoopBuilderOld setQuantity(int quantity) {
        this.quantity = quantity;
        return this;
    }

    public void init(StopCondition stopCondition, int range) {
        this.stopCondition = stopCondition;
        parameters.init(range);
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

    public ThreadLoopBuilderOld parseThreadLoop(ParseArgument args) {
        return switch (args.getCurrent()) {
            case "-delete-leafs" -> {
                args.next();
                yield this.setType(ThreadLoopType.DELETE_LEAFS).setParameters(new DeleteLeafsParameters());
            }
            case "-temp-oper", "-temporary-operation" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
//                parameters.keyGeneratorBuilder = KeyGeneratorBuilderOld.parseKeyGenerator(args);
                yield this.setType(ThreadLoopType.TEMPORARY_OPERATIONS).setParameters(parameters);
            }
            case "-temp-oper-2", "-temporary-operation-2" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
//                parameters.keyGeneratorBuilder = KeyGeneratorBuilderOld.parseKeyGenerator(args);
                yield this.setType(ThreadLoopType.TEMPORARY_OPERATIONS_2).setParameters(parameters);
            }
            default -> {
                DefaultThreadLoopParameters parameters = new DefaultThreadLoopParameters();
//                parameters.keyGeneratorBuilder = KeyGeneratorBuilderOld.parseKeyGenerator(args);
                yield this.setType(ThreadLoopType.DEFAULT).setParameters(parameters);
            }
        };
    }


    public static ThreadLoopBuilderOld parseAndCreateThreadLoop(ParseArgument args) {
        return switch (args.getCurrent()) {
            case "-delete-leafs" -> {
                args.next();
                yield new ThreadLoopBuilderOld(ThreadLoopType.DELETE_LEAFS);
            }
            case "-temp-oper", "-temporary-operation" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
//                parameters.keyGeneratorBuilder = KeyGeneratorBuilderOld.parseKeyGenerator(args);
                yield new ThreadLoopBuilderOld(ThreadLoopType.TEMPORARY_OPERATIONS).setParameters(parameters);
            }
            case "-temp-oper-2", "-temporary-operation-2" -> {
                args.next();
                TemporaryOperationsThreadLoopParameters parameters = new TemporaryOperationsThreadLoopParameters();
//                parameters.keyGeneratorBuilder = KeyGeneratorBuilderOld.parseKeyGenerator(args);
                yield new ThreadLoopBuilderOld(ThreadLoopType.TEMPORARY_OPERATIONS_2).setParameters(parameters);
            }
            default -> {
                DefaultThreadLoopParameters parameters = new DefaultThreadLoopParameters();
//                parameters.keyGeneratorBuilder = KeyGeneratorBuilderOld.parseKeyGenerator(args);
                yield new ThreadLoopBuilderOld().setParameters(parameters);
            }
        };
    }
}
