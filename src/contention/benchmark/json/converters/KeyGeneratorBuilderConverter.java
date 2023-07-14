package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorParameters;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorType;

import java.lang.reflect.Type;

public class KeyGeneratorBuilderConverter implements JsonSerializer<KeyGeneratorBuilder>, JsonDeserializer<KeyGeneratorBuilder> {
    @Override
    public KeyGeneratorBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        KeyGeneratorType keyGenType = context.deserialize(object.get("type"), KeyGeneratorType.class);

        Class<?> parametersClass;
        try {
            parametersClass = Class.forName(object.get("ParametersClassName").getAsString());
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        KeyGeneratorParameters parameters = context.deserialize(object.get("parameters"), parametersClass);

        return new KeyGeneratorBuilder(keyGenType).setParameters(parameters);
    }

    @Override
    public JsonElement serialize(KeyGeneratorBuilder keyGeneratorBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
        object.addProperty("ParametersClassName", keyGeneratorBuilder.parameters.getClass().getName());
        object.add("type", context.serialize(keyGeneratorBuilder.type));
        object.add("parameters", context.serialize(keyGeneratorBuilder.parameters));
        return object;
    }
}
