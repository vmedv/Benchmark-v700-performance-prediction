package trees.lockfree;

import contention.abstractions.CompositionalIntSet;
import tree.LockFreeSet;

import java.util.Collection;

public class LockFreeSetAdapter implements CompositionalIntSet {
    private final LockFreeSet<Integer> set;

    public LockFreeSetAdapter() {
        set = new LockFreeSet<>();
    }

    @Override
    public void fill(int range, long size) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean addInt(int x) {
        return set.insert(x);
    }

    @Override
    public boolean removeInt(int x) {
        return set.delete(x);
    }

    @Override
    public boolean containsInt(int x) {
        return set.contains(x);
    }

    @Override
    public Object getInt(int x) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean addAll(Collection<Integer> c) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean removeAll(Collection<Integer> c) {
        throw new UnsupportedOperationException();
    }

    @Override
    public int size() {
        return set.count(Integer.MIN_VALUE, Integer.MAX_VALUE);
    }

    @Override
    public void clear() {

    }

    @Override
    public Object putIfAbsent(int x, int y) {
        throw new UnsupportedOperationException();
    }
}
