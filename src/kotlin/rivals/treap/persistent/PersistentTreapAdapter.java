package kotlin.rivals.treap.persistent;

import contention.abstractions.CompositionalIntSet;
import rivals.treap.persistent.PersistentTreap;

import java.util.Collection;

public class PersistentTreapAdapter implements CompositionalIntSet {
    private final PersistentTreap<Integer> set;

    public PersistentTreapAdapter() {
        set = new PersistentTreap<>();
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
