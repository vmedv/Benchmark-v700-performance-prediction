package contention.benchmark.workload.data.map.abstractions;

public abstract class DataMapBuilder {
    private static int idCounter = 0;
    public int id = idCounter++;

    public abstract DataMapBuilder init(int range);
    public abstract DataMap build();

    public abstract StringBuilder toStringBuilder();

    private DataMap dataMap;

    public DataMap getOrBuild() {
        if (dataMap == null)
            dataMap = this.build();
        return dataMap;
    }

    public void reset() {
        dataMap = null;
    }
}
