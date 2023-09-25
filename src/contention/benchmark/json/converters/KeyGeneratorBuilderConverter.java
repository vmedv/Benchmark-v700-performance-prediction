package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.workload.args.generators.abstractions.ArgsGeneratorBuilder;

import java.lang.reflect.Type;

public class KeyGeneratorBuilderConverter implements JsonSerializer<ArgsGeneratorBuilder>, JsonDeserializer<ArgsGeneratorBuilder> {
    @Override
    public ArgsGeneratorBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        ArgsGeneratorBuilder argsGeneratorBuilder = null;

        try {
            Class<?> dataMapBuilderClass = Class.forName(object.get("ClassName").getAsString());
            argsGeneratorBuilder = context.deserialize(object.get("KeyGeneratorBuilder"), dataMapBuilderClass);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        return argsGeneratorBuilder;
    }

    @Override
    public JsonElement serialize(ArgsGeneratorBuilder argsGeneratorBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = context.serialize(argsGeneratorBuilder).getAsJsonObject();
        object.addProperty("ClassName", argsGeneratorBuilder.getClass().getName());
        object.addProperty("AClassName", "test");
        object.addProperty("ZClassName", "test");
//        object.addProperty("ClassName", keyGeneratorBuilder.getClass().getName());
//        object.add("KeyGeneratorBuilder", context.serialize(keyGeneratorBuilder));
        return object;
    }
}
