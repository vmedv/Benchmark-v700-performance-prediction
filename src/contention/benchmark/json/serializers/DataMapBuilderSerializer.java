package contention.benchmark.json.serializers;

import com.google.gson.*;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;

import java.lang.reflect.Type;
import java.util.HashMap;
import java.util.Map;

public class DataMapBuilderSerializer implements JsonSerializer<DataMapBuilder>, JsonDeserializer<DataMapBuilder> {
    private final Map<Integer, DataMapBuilder> builders = new HashMap<>();

    @Override
    public DataMapBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        Class<?> builderClass;
        try {
            builderClass = Class.forName(object.get("ClassName").getAsString());
            object.remove("ClassName");
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        DataMapBuilder builder;
        int id = object.get("id").getAsInt();

        if ((builder = builders.get(id)) == null) {
            builder = context.deserialize(object, builderClass);
            builders.put(id, builder);
        }

        return builder;
    }

    @Override
    public JsonElement serialize(DataMapBuilder builder, Type type, JsonSerializationContext context) {
        JsonObject object = context.serialize(builder).getAsJsonObject();
        object.addProperty("ClassName", builder.getClass().getName());
        return object;
    }
}
