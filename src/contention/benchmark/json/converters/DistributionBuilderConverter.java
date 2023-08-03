package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;

import java.lang.reflect.Type;

public class DistributionBuilderConverter implements JsonSerializer<DistributionBuilder>, JsonDeserializer<DistributionBuilder> {
    private <T> T getClassByName(JsonElement json, Class<T> tClass, JsonDeserializationContext context) {
        if (json == null) {
            return null;
        } else {
            return context.deserialize(json, tClass);
        }
    }

    @Override
    public DistributionBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        DistributionBuilder dataMapBuilder;

        try {
            Class<?> dataMapBuilderClass = Class.forName(object.get("ClassName").getAsString());
            dataMapBuilder = context.deserialize(object.get("DistributionBuilder"), dataMapBuilderClass);
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
    public JsonElement serialize(DistributionBuilder distributionBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
        object.addProperty("ClassName", distributionBuilder.getClass().getName());
        object.add("DistributionBuilder", context.serialize(distributionBuilder));
        return object;
    }
}
