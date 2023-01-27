package contention.benchmark.ThreadLoops.workloads;

import contention.abstractions.CompositionalMap;
import contention.abstractions.KeyGenerator;
import contention.abstractions.Parameters;
import contention.benchmark.ThreadLoops.ThreadMapLoop;

import java.lang.reflect.Method;
import java.util.*;

public class DeleteSpeedTest extends ThreadMapLoop {
    public DeleteSpeedTest(short myThreadNum, CompositionalMap<Integer, Integer> bench, Method[] methods, Parameters parameters) {
        super(myThreadNum, bench, methods, null, parameters);
    }

    private List<Integer> vertices;
    private int lastLayer;


    @Override
    public void run() {
        long startTime = System.currentTimeMillis();
        long startNanoTime = System.nanoTime();

        for (int i = lastLayer; i < vertices.size(); i++) {
            bench.remove(vertices.get(i));
        }

        while (!bench.isEmpty());

        long endTime = System.currentTimeMillis();
        long endNanoTime = System.nanoTime();

        long elapsedTime = endTime - startTime;
        long elapsedNanoTime = endNanoTime - startNanoTime;

        System.out.println("\n===========================================================\n"
                + "Delete time:         \t" + elapsedTime + "\n"
                + "Delete nano time:    \t" + elapsedNanoTime
                + "\n===========================================================\n");

        // System.out.println(numAdd + " " + numRemove + " " + failures);
        this.getCount = CompositionalMap.counts.get().getCount;
        this.nodesTraversed = CompositionalMap.counts.get().nodesTraversed;
        this.structMods = CompositionalMap.counts.get().structMods;
        System.out.println("Thread #" + myThreadNum + " finished.");
    }

    @Override
    public void prefill() {
        int size = parameters.size;
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
            bench.putIfAbsent(nextVert, nextVert);

            if (next.left == next.right) {
                continue;
            }

            vertQueue.add(new Range(next.left, nextVert - 1));
            vertQueue.add(new Range(nextVert + 1, next.right));
        }

        for (int i = 0, deg = 1; i + deg < size; deg <<= 1) {
            for (int j = 0; j < deg; j++) {
                bench.remove(vertices.get(i + j));
            }
            i += deg;
            lastLayer = i;
        }
    }

    private static class Range {
        public int left;
        public int right;

        public Range(int left, int right) {
            this.left = left;
            this.right = right;
        }
    }
}
