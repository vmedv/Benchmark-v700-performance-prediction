package contention.benchmark.ThreadLoops.abstractions;

import contention.abstractions.CompositionalMap;
import contention.abstractions.DataStructure;
import contention.benchmark.ThreadStatistic;
import contention.benchmark.stop.condition.StopCondition;

import java.lang.reflect.Method;
import java.util.Collection;

public abstract class ThreadLoopAbstract implements ThreadLoop {
    protected StopCondition stopCondition;

    protected final DataStructure<Integer> dataStructure;

    /**
     * The number of the current thread
     */
    protected final int myThreadNum;
    /**
     * The pool of methods that can run
     */
    protected Method[] methods;
    /**
     * The stop flag, indicating whether the loop is over
     */
    protected volatile boolean stop = false;
    public ThreadStatistic stats = new ThreadStatistic();
//    public long numAdd = 0;
//    public long numRemove = 0;
//    public long numAddAll = 0;
//    public long numRemoveAll = 0;
//    public long numSize = 0;
//    public long numContains = 0;
    /**
     * The counter of the false-returning operations
     */
//    public long failures = 0;
    /**
     * The counter of the thread operations
     */
//    public long total = 0;
    /**
     * The counter of aborts
     */
//    public long aborts = 0;
//
//    public long getCount;
//    public long nodesTraversed;
//    public long structMods;

    protected ThreadLoopAbstract(int myThreadNum, DataStructure<Integer> dataStructure,
                                 Method[] methods, StopCondition stopCondition) {
        this.myThreadNum = myThreadNum;
        this.methods = methods;
        this.dataStructure = dataStructure;
        this.stopCondition = stopCondition;
    }

//    public long getNumAdd() {
//        return numAdd;
//    }
//
//    public void setNumAdd(long numAdd) {
//        this.numAdd = numAdd;
//    }
//
//    public long getNumRemove() {
//        return numRemove;
//    }
//
//    public void setNumRemove(long numRemove) {
//        this.numRemove = numRemove;
//    }
//
//    public long getNumAddAll() {
//        return numAddAll;
//    }
//
//    public void setNumAddAll(long numAddAll) {
//        this.numAddAll = numAddAll;
//    }
//
//    public long getNumRemoveAll() {
//        return numRemoveAll;
//    }
//
//    public void setNumRemoveAll(long numRemoveAll) {
//        this.numRemoveAll = numRemoveAll;
//    }
//
//    public long getNumSize() {
//        return numSize;
//    }
//
//    public void setNumSize(long numSize) {
//        this.numSize = numSize;
//    }
//
//    public long getNumContains() {
//        return numContains;
//    }
//
//    public void setNumContains(long numContains) {
//        this.numContains = numContains;
//    }
//
//    public long getFailures() {
//        return failures;
//    }
//
//    public void setFailures(long failures) {
//        this.failures = failures;
//    }
//
//    public long getTotal() {
//        return total;
//    }
//
//    public void setTotal(long total) {
//        this.total = total;
//    }
//
//    public long getAborts() {
//        return aborts;
//    }
//
//    public void setAborts(long aborts) {
//        this.aborts = aborts;
//    }
//
//    public long getGetCount() {
//        return getCount;
//    }
//
//    public void setGetCount(long getCount) {
//        this.getCount = getCount;
//    }
//
//    public long getNodesTraversed() {
//        return nodesTraversed;
//    }
//
//    public void setNodesTraversed(long nodesTraversed) {
//        this.nodesTraversed = nodesTraversed;
//    }
//
//    public long getStructMods() {
//        return structMods;
//    }

    public void stopThread() {
        stop = true;
    }

//    public void setStructMods(long structMods) {
//        this.stats.structMods = structMods;
//    }

    public void printDataStructure() {
        System.out.println(dataStructure.toString());
    }

    public void run() {
        while (!stopCondition.isStopped(myThreadNum)) {
            step();
            stats.total++;

            assert stats.total == stats.failures + stats.numContains + stats.numSize + stats.numRemove
                    + stats.numAdd + stats.numRemoveAll + stats.numAddAll;
        }
        // System.out.println(numAdd + " " + numRemove + " " + failures);
        this.stats.getCount = CompositionalMap.counts.get().getCount;
        this.stats.nodesTraversed = CompositionalMap.counts.get().nodesTraversed;
        this.stats.structMods = CompositionalMap.counts.get().structMods;
        System.out.println("Thread #" + myThreadNum + " finished.");
    }

    public Integer insert(int key) {
        Integer result;
        if ((result = dataStructure.insert(key)) == null) {
            stats.numAdd++;
        } else {
            stats.failures++;
        }
        return result;
    }

    public Integer remove(int key) {
        Integer result;
        if ((result = dataStructure.remove(key)) != null) {
            stats.numRemove++;
        } else {
            stats.failures++;
        }
        return result;
    }


    public Integer get(int key) {
        Integer result;
        if ((result = dataStructure.get(key)) != null) {
            stats.numContains++;
        } else {
            stats.failures++;
        }
        return result;
    }

    public boolean removeAll(Collection<Integer> c) {
        boolean result = false;
        try {
            result = dataStructure.removeAll(c);
        } catch (Exception e) {
            System.err.println("Unsupported writeAll operations! Leave the default value of the numWriteAlls parameter (0).");
        }

        if (result) {
            stats.numRemoveAll++;
        } else {
            stats.failures++;
        }
        return result;
    }

    public int size() {
        stats.numSize++;
        return dataStructure.size();
    }

}
