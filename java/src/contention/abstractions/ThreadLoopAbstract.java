package contention.abstractions;

public abstract class ThreadLoopAbstract implements ThreadLoop {
    protected final KeyGenerator keygen;
    protected final Parameters parameters;
    /**
     * The stop flag, indicating whether the loop is over
     */
    protected volatile boolean stop = false;
    public long numAdd = 0;
    public long numRemove = 0;
    public long numAddAll = 0;
    public long numRemoveAll = 0;
    public long numSize = 0;
    public long numContains = 0;
    /**
     * The counter of the false-returning operations
     */
    public long failures = 0;
    /**
     * The counter of the thread operations
     */
    public long total = 0;
    /**
     * The counter of aborts
     */
    public long aborts = 0;

    public long getCount;
    public long nodesTraversed;
    public long structMods;

    protected ThreadLoopAbstract(KeyGenerator keygen, Parameters parameters) {
        this.keygen = keygen;
        this.parameters = parameters;
    }

    public long getNumAdd() {
        return numAdd;
    }

    public void setNumAdd(long numAdd) {
        this.numAdd = numAdd;
    }

    public long getNumRemove() {
        return numRemove;
    }

    public void setNumRemove(long numRemove) {
        this.numRemove = numRemove;
    }

    public long getNumAddAll() {
        return numAddAll;
    }

    public void setNumAddAll(long numAddAll) {
        this.numAddAll = numAddAll;
    }

    public long getNumRemoveAll() {
        return numRemoveAll;
    }

    public void setNumRemoveAll(long numRemoveAll) {
        this.numRemoveAll = numRemoveAll;
    }

    public long getNumSize() {
        return numSize;
    }

    public void setNumSize(long numSize) {
        this.numSize = numSize;
    }

    public long getNumContains() {
        return numContains;
    }

    public void setNumContains(long numContains) {
        this.numContains = numContains;
    }

    public long getFailures() {
        return failures;
    }

    public void setFailures(long failures) {
        this.failures = failures;
    }

    public long getTotal() {
        return total;
    }

    public void setTotal(long total) {
        this.total = total;
    }

    public long getAborts() {
        return aborts;
    }

    public void setAborts(long aborts) {
        this.aborts = aborts;
    }

    public long getGetCount() {
        return getCount;
    }

    public void setGetCount(long getCount) {
        this.getCount = getCount;
    }

    public long getNodesTraversed() {
        return nodesTraversed;
    }

    public void setNodesTraversed(long nodesTraversed) {
        this.nodesTraversed = nodesTraversed;
    }

    public long getStructMods() {
        return structMods;
    }

    public void stopThread() {
        stop = true;
    }

    public void setStructMods(long structMods) {
        this.structMods = structMods;
    }
}
