package contention.benchmark.datasctucrure;

import contention.abstractions.CompositionalMap;
import contention.abstractions.DataStructure;

import java.util.Collection;

public class MapDataStructure<K> implements DataStructure<K> {
    private final CompositionalMap<K, K> dataStructure;

    public MapDataStructure(CompositionalMap<K, K> dataStructure) {
        this.dataStructure = dataStructure;
    }

    @Override
    public K insert(K key) {
        return dataStructure.putIfAbsent(key, key);
    }

    @Override
    public K remove(K key) {
        return dataStructure.remove(key);
    }

    @Override
    public K get(K key) {
        return dataStructure.get(key);
    }

    @Override
    public boolean removeAll(Collection<K> c) {
        return false;
    }

    @Override
    public int size() {
        return dataStructure.size();
    }

    @Override
    public void clear() {
        dataStructure.clear();
    }

    @Override
    public String toString() {
        return dataStructure.toString();
    }

    @Override
    public Object getDataStructure() {
        return dataStructure;
    }
}
