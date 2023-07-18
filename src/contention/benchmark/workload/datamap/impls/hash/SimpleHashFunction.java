package contention.benchmark.workload.datamap.impls.hash;

public class SimpleHashFunction implements HashFunction {

    @Override
    public int hash(int index) {
        return ((Integer) index).hashCode();
    }
}
