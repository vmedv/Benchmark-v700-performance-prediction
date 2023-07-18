package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopBuilderOld;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopParameters;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopType;

import java.lang.reflect.Type;

public class ThreadLoopBuilderConverter implements JsonSerializer<ThreadLoopBuilderOld>, JsonDeserializer<ThreadLoopBuilderOld> {
    @Override
    public ThreadLoopBuilderOld deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
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

        return new ThreadLoopBuilderOld(threadLoopType).setParameters(parameters).setQuantity(quantity);
    }

    @Override
    public JsonElement serialize(ThreadLoopBuilderOld threadLoopBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
        object.addProperty("ParametersClassName", threadLoopBuilder.parameters.getClass().getName());
        object.add("type", context.serialize(threadLoopBuilder.type));
        object.add("quantity", context.serialize(threadLoopBuilder.quantity));
        object.add("parameters", context.serialize(threadLoopBuilder.parameters));
        return object;
    }
}

