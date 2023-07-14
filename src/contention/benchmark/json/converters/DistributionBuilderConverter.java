package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.distributions.abstractions.DistributionBuilder;
import contention.benchmark.distributions.abstractions.DistributionParameters;
import contention.benchmark.distributions.abstractions.DistributionType;

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

        DistributionType distributionType = context.deserialize(object.get("type"), DistributionType.class);

        Class<?> parametersClass;
        DistributionParameters parameters = null;
        try {
            if (object.get("ParametersClassName") != null) {
                parametersClass = Class.forName(object.get("ParametersClassName").getAsString());
                parameters = context.deserialize(object.get("parameters"), parametersClass);
            }
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

//        DistributionParameters parameters = context.deserialize(object.get("parameters"), parametersClass);

        return new DistributionBuilder(distributionType).setParameters(parameters);
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
//        object.addProperty("ParametersClassName", distributionBuilder.parameters.getClass().getName());
        object.addProperty("ParametersClassName", getClassName(distributionBuilder.parameters));
        object.add("type", context.serialize(distributionBuilder.type));
        object.add("parameters", context.serialize(distributionBuilder.parameters));
        return object;
    }
}
