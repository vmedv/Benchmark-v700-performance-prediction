package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.workload.stop.condition.StopCondition;

import java.lang.reflect.Type;

public class StopConditionConverter implements JsonSerializer<StopCondition>, JsonDeserializer<StopCondition> {

    private <T> T getClassByName(JsonElement json, Class<T> tClass, JsonDeserializationContext context) {
        if (json == null) {
            return null;
        } else {
            return context.deserialize(json, tClass);
        }
    }

    @Override
    public StopCondition deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        Class<?> stopConditionClass;
        StopCondition stopCondition = null;
        try {
            if (object.get("ClassName") != null) {
                stopConditionClass = Class.forName(object.get("ClassName").getAsString());
                stopCondition = context.deserialize(object.get("StopCondition"), stopConditionClass);
            }
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

        return stopCondition;
    }

    private String getClassName(Object object) {
        if (object == null) {
            return null;
        } else {
            return object.getClass().getName();
        }
    }

    @Override
    public JsonElement serialize(StopCondition stopCondition, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
//        object.addProperty("ParametersClassName", stopCondition.parameters.getClass().getName());
        JsonElement element = context.serialize(stopCondition);

        object.addProperty("ClassName", getClassName(stopCondition));
        object.add("StopCondition", context.serialize(stopCondition));
        return object;
    }
}
