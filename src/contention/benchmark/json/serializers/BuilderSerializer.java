package contention.benchmark.json.serializers;

import com.google.gson.*;

import java.lang.reflect.Type;

public class BuilderSerializer<T> implements JsonSerializer<T>, JsonDeserializer<T> {


    @Override
    public T deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        T builder;

        try {
            Class<?> builderClass = Class.forName(object.get("ClassName").getAsString());
            object.remove("ClassName");
            builder = context.deserialize(object, builderClass);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        return builder;
    }

    @Override
    public JsonElement serialize(T builder, Type type, JsonSerializationContext context) {
        JsonObject object = context.serialize(builder).getAsJsonObject();
        object.addProperty("ClassName", builder.getClass().getName());
        return object;
    }
}
