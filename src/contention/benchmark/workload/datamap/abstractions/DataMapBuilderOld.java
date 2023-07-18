package contention.benchmark.workload.datamap.abstractions;

import contention.abstractions.ParseArgument;
import contention.benchmark.workload.datamap.impls.ArrayDataMap;
import contention.benchmark.workload.datamap.impls.HashDataMap;
import contention.benchmark.workload.datamap.impls.IdDataMap;

import java.util.List;

public class DataMapBuilderOld {
    public static List<DataMap> dataMaps;
    public int id;

    public DataMapType type;
    public DataMapParameters parameters;

    public DataMapBuilderOld() {
        type = DataMapType.ID;
    }

    public DataMapBuilderOld(DataMapType type) {
        this.type = type;
    }

    public static void setDataMaps(List<DataMap> dataMaps) {
        DataMapBuilderOld.dataMaps = dataMaps;
    }

    public DataMapBuilderOld setId(int id) {
        this.id = id;
        return this;
    }

    public DataMapBuilderOld setType(DataMapType type) {
        this.type = type;
        return this;
    }

    public DataMapBuilderOld setParameters(DataMapParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public boolean parseDataMap(ParseArgument args) {
        switch (args.getCurrent()) {
            case "-id" -> this.setType(DataMapType.ID);
            case "-array" -> this.setType(DataMapType.ARRAY);
            case "-hash" -> this.setType(DataMapType.HASH); //TODO parse hash type
            default -> {
                return false;
            }
        }
        return true;
    }

    public DataMap get() {
        return dataMaps.get(id);
    }

    public DataMap build(int range) {
        return switch (type) {
            case STATIC -> dataMaps.get(id);
            case ID -> new IdDataMap();
            case ARRAY -> new ArrayDataMap(range);
            case HASH -> new HashDataMap(null);
        };
    }
}
