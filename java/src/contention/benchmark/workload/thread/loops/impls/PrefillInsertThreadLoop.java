package contention.benchmark.workload.thread.loops.impls;

import contention.abstractions.DataStructure;
import contention.benchmark.workload.args.generators.abstractions.ArgsGenerator;
import contention.benchmark.workload.stop.condition.StopCondition;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoop;

import java.lang.reflect.Method;
import java.util.Random;

public class PrefillInsertThreadLoop extends ThreadLoop {
    private final ArgsGenerator argsGenerator;
    private final int numberOfAttempts;

    public PrefillInsertThreadLoop(int threadId, DataStructure<Integer> dataStructure,
                                      Method[] methods, StopCondition stopCondition,
                                      ArgsGenerator argsGenerator, int numberOfAttempts) {
        super(threadId, dataStructure, methods, stopCondition);
        this.argsGenerator = argsGenerator;
        this.numberOfAttempts = numberOfAttempts;
    }

    @Override
    public void step() {
        int counter = 0;
        Integer value;
        do {
            int key = argsGenerator.nextInsert();
            value = executeInsert(key);
            ++counter;
        } while (value != null && counter < numberOfAttempts);

        if (value != null) {
            System.err.println("WARNING: PrefillInsertThreadLoop with threadId=" + threadId
                    + " have not inserted a new key. Number of attempts is: "
                    + numberOfAttempts + "\n");
        }
    }
}
