package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;

import java.lang.reflect.Type;

public class DataMapBuilderConverter implements JsonSerializer<DataMapBuilder>, JsonDeserializer<DataMapBuilder> {

    private <T> T getClassByName(JsonElement json, Class<T> tClass, JsonDeserializationContext context) {
        if (json == null) {
            return null;
        } else {
            return context.deserialize(json, tClass);
        }
    }

    @Override
    public DataMapBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        DataMapBuilder dataMapBuilder;

        try {
            Class<?> dataMapBuilderClass = Class.forName(object.get("ClassName").getAsString());
            dataMapBuilder = context.deserialize(object.get("DataMapBuilder"), dataMapBuilderClass);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        return dataMapBuilder;
    }

    private String getClassName(Object object) {
        if (object == null) {
            return null;
        } else {
            return object.getClass().getName();
        }
    }

    @Override
    public JsonElement serialize(DataMapBuilder dataMapBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
        object.addProperty("ClassName", dataMapBuilder.getClass().getName());
        object.add("DataMapBuilder", context.serialize(dataMapBuilder));
        return object;
    }
}
