package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopBuilder;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopParameters;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopType;

import java.lang.reflect.Type;

public class ThreadLoopBuilderConverter implements JsonSerializer<ThreadLoopBuilder>, JsonDeserializer<ThreadLoopBuilder> {
    @Override
    public ThreadLoopBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        ThreadLoopType threadLoopType = context.deserialize(object.get("type"), ThreadLoopType.class);
        int quantity = context.deserialize(object.get("quantity"), Integer.class);

        Class<?> parametersClass;
        try {
            parametersClass = Class.forName(object.get("ParametersClassName").getAsString());
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        ThreadLoopParameters parameters = context.deserialize(object.get("parameters"), parametersClass);

        return new ThreadLoopBuilder(threadLoopType).setParameters(parameters).setQuantity(quantity);
    }

    @Override
    public JsonElement serialize(ThreadLoopBuilder threadLoopBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
        object.addProperty("ParametersClassName", threadLoopBuilder.parameters.getClass().getName());
        object.add("type", context.serialize(threadLoopBuilder.type));
        object.add("quantity", context.serialize(threadLoopBuilder.quantity));
        object.add("parameters", context.serialize(threadLoopBuilder.parameters));
        return object;
    }
}

