package contention.benchmark.workload.datamap.abstractions;

public abstract class DataMapBuilder {
    public abstract DataMapBuilder init(int range);
    public abstract DataMap build();

    public abstract StringBuilder toStringBuilder();

    private DataMap dataMap;

    public DataMap getOrBuild() {
        if (dataMap == null)
            dataMap = this.build();
        return dataMap;
    }
}
