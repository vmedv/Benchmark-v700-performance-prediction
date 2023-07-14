package contention.benchmark.ThreadLoops.impls;

import contention.abstractions.*;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopAbstract;
import contention.benchmark.ThreadLoops.parameters.DeleteLeafsParameters;
import contention.benchmark.stop.condition.StopCondition;
import contention.benchmark.tools.Range;

import java.lang.reflect.Method;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.atomic.AtomicInteger;

public class DeleteLeafsWorkload extends ThreadLoopAbstract {
    private WorkloadEpoch workloadEpoch = WorkloadEpoch.DELETE_INTERNAL;
    private static List<Integer> vertices;
    private static int lastLayer;

    public DeleteLeafsWorkload(int myThreadNum, DataStructure<Integer> dataStructure,
//                               CompositionalMap<Integer, Integer> bench,
                               Method[] methods, StopCondition stopCondition,
                               DeleteLeafsParameters parameters) {
        super(myThreadNum, dataStructure, methods, stopCondition);
        int size = parameters.range;
        vertices = new ArrayList<>(size);
        Queue<Range> vertQueue = new ArrayDeque<>();

        vertQueue.add(new Range(1, size));

        while (!vertQueue.isEmpty()) {
            Range next = vertQueue.remove();

            if (next.left > next.right) {
                continue;
            }

            int nextVert = (next.left + next.right) / 2;

            vertices.add(nextVert);
//            bench.putIfAbsent(nextVert, nextVert);

            if (next.left == next.right) {
                continue;
            }

            vertQueue.add(new Range(next.left, nextVert - 1));
            vertQueue.add(new Range(nextVert + 1, next.right));
        }

        for (int i = 0, deg = 1; i + deg < size; deg <<= 1) {
//            for (int j = 0; j < deg; j++) {
//                bench.remove(vertices.get(i + j));
//            }
            i += deg;
            lastLayer = i;
        }
    }

    @Override
    public void run() {
        curIndex = 0;
        super.run();
    }

    private int curIndex;

    @Override
    public void step() {
        switch (workloadEpoch) {
            case ADD -> {
                int key = vertices.get(curIndex++);
                insert(key);

                if (curIndex >= vertices.size()) {
                    workloadEpoch = WorkloadEpoch.DELETE_INTERNAL;
                    curIndex = 0;
                }
            }
            case DELETE_INTERNAL -> {
                int key = vertices.get(curIndex++);
                remove(key);

                if (curIndex >= lastLayer) {
                    workloadEpoch = WorkloadEpoch.DELETE_LEAF;
                    curIndex = lastLayer;
                }
            }
            case DELETE_LEAF -> {
                int key = vertices.get(curIndex++);
                remove(key);

                if (curIndex >= vertices.size()) {
                    workloadEpoch = WorkloadEpoch.ADD;
                    curIndex = 0;
                }
            }
        }
    }

    @Override
    public void prefill(AtomicInteger prefillSize) {
        for (int key : vertices) {
            insert(key);
        }
    }

    private enum WorkloadEpoch {
        ADD, DELETE_INTERNAL, DELETE_LEAF
    }
}
