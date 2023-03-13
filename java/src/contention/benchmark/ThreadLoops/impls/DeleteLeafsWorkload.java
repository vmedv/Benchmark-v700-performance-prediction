package contention.benchmark.ThreadLoops.impls;

import contention.abstractions.*;
import contention.benchmark.tools.Range;

import java.lang.reflect.Method;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.atomic.AtomicInteger;

public class DeleteLeafsWorkload extends ThreadLoopAbstract {
    /**
     * The instance of the running benchmark
     */
    public final CompositionalMap<Integer, Integer> bench;
    private WorkloadEpoch workloadEpoch = WorkloadEpoch.DELETE_INTERNAL;
    private static List<Integer> vertices;
    private static int lastLayer;

    public DeleteLeafsWorkload(short myThreadNum, CompositionalMap<Integer, Integer> bench,
                               Method[] methods, Parameters parameters) {
        super(myThreadNum, methods, null);
        this.bench = bench;
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
        int curIndex = 0;

        while (!stop) {
            Integer a, b;

            switch (workloadEpoch) {
                case ADD -> {
                    int newInt = vertices.get(curIndex++);

                    if ((a = bench.putIfAbsent(newInt, newInt)) == null) {
                        numAdd++;
                    } else {
                        failures++;
                    }

                    if (curIndex >= vertices.size()) {
                        workloadEpoch = WorkloadEpoch.DELETE_INTERNAL;
                        curIndex = 0;
                    }
                }
                case DELETE_INTERNAL -> {
                    int newInt = vertices.get(curIndex++);

                    if ((a = bench.remove(newInt)) != null) {
                        numRemove++;
                    } else {
                        failures++;
                    }

                    if (curIndex >= lastLayer) {
                        workloadEpoch = WorkloadEpoch.DELETE_LEAF;
                        curIndex = lastLayer;
                    }
                }
                case DELETE_LEAF -> {
                    int newInt = vertices.get(curIndex++);

                    if ((a = bench.remove(newInt)) != null) {
                        numRemove++;
                    } else {
                        failures++;
                    }

                    if (curIndex >= vertices.size()) {
                        workloadEpoch = WorkloadEpoch.ADD;
                        curIndex = 0;
                    }
                }
            }

            total++;

            assert total == failures + numContains + numSize + numRemove
                    + numAdd + numRemoveAll + numAddAll;
        }


        // System.out.println(numAdd + " " + numRemove + " " + failures);
        this.getCount = CompositionalMap.counts.get().getCount;
        this.nodesTraversed = CompositionalMap.counts.get().nodesTraversed;
        this.structMods = CompositionalMap.counts.get().structMods;
        System.out.println("Thread #" + myThreadNum + " finished.");
    }

    @Override
    public void prefill(AtomicInteger prefillSize) {
        for (int newInt : vertices) {
            bench.putIfAbsent(newInt, newInt);
        }
    }

    private enum WorkloadEpoch {
        ADD, DELETE_INTERNAL, DELETE_LEAF
    }
}
