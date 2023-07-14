package contention.benchmark.datamap.abstractions;

import contention.abstractions.ParseArgument;
import contention.benchmark.datamap.impls.ArrayDataMap;
import contention.benchmark.datamap.impls.HashDataMap;
import contention.benchmark.datamap.impls.IdDataMap;

import java.util.List;

public class DataMapBuilder {
    public static List<DataMap> dataMaps;
    public int id;

    public DataMapType type;
    public DataMapParameters parameters;

    public DataMapBuilder() {
        type = DataMapType.ID;
    }

    public DataMapBuilder(DataMapType type) {
        this.type = type;
    }

    public static void setDataMaps(List<DataMap> dataMaps) {
        DataMapBuilder.dataMaps = dataMaps;
    }

    public DataMapBuilder setId(int id) {
        this.id = id;
        return this;
    }

    public DataMapBuilder setType(DataMapType type) {
        this.type = type;
        return this;
    }

    public DataMapBuilder setParameters(DataMapParameters parameters) {
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
