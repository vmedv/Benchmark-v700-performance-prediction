package contention.abstractions;

public interface MutableDistribution extends Distribution {
    void setMaxKey(int maxKey);

    int next(int maxKey);
}
