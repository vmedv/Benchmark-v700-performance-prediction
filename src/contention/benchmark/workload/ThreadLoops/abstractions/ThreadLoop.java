package contention.benchmark.workload.ThreadLoops.abstractions;

import java.util.concurrent.atomic.AtomicInteger;

public interface ThreadLoop extends Runnable {
//    long getNumAdd();
//
//    void setNumAdd(long numAdd);
//
//    long getNumRemove();
//
//    void setNumRemove(long numRemove);
//
//    long getNumAddAll();
//
//    void setNumAddAll(long numAddAll);
//
//    long getNumRemoveAll();
//
//    void setNumRemoveAll(long numRemoveAll);
//
//    long getNumSize();
//
//    void setNumSize(long numSize);
//
//    long getNumContains();
//
//    void setNumContains(long numContains);
//
//    long getFailures();
//
//    void setFailures(long failures);
//
//    long getTotal();
//
//    void setTotal(long total);
//
//    long getAborts();
//
//    void setAborts(long aborts);
//
//    long getGetCount();
//
//    void setGetCount(long getCount);
//
//    long getNodesTraversed();
//
//    void setNodesTraversed(long nodesTraversed);
//
//    long getStructMods();
//
//    void setStructMods(long structMods);
//
    void stopThread();

    void run();

    void step();

    void prefill(AtomicInteger prefillSize);


}
