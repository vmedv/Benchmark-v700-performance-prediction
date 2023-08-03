package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;

import java.lang.reflect.Type;

public class KeyGeneratorBuilderConverter implements JsonSerializer<KeyGeneratorBuilder>, JsonDeserializer<KeyGeneratorBuilder> {
    @Override
    public KeyGeneratorBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        KeyGeneratorBuilder keyGeneratorBuilder = null;

        try {
            Class<?> dataMapBuilderClass = Class.forName(object.get("ClassName").getAsString());
            keyGeneratorBuilder = context.deserialize(object.get("KeyGeneratorBuilder"), dataMapBuilderClass);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        return keyGeneratorBuilder;
    }

    @Override
    public JsonElement serialize(KeyGeneratorBuilder keyGeneratorBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = context.serialize(keyGeneratorBuilder).getAsJsonObject();
        object.addProperty("ClassName", keyGeneratorBuilder.getClass().getName());
        object.addProperty("AClassName", "test");
        object.addProperty("ZClassName", "test");
//        object.addProperty("ClassName", keyGeneratorBuilder.getClass().getName());
//        object.add("KeyGeneratorBuilder", context.serialize(keyGeneratorBuilder));
        return object;
    }
}
