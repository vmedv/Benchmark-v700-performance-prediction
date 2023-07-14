package contention.benchmark.json.converters;

import com.google.gson.*;
import contention.benchmark.datamap.abstractions.DataMapBuilder;
import contention.benchmark.datamap.abstractions.DataMapParameters;
import contention.benchmark.datamap.abstractions.DataMapType;

import java.lang.reflect.Type;

public class DataMapBuilderConverter implements JsonSerializer<DataMapBuilder>, JsonDeserializer<DataMapBuilder> {

    private <T> T getClassByName(JsonElement json, Class<T> tClass, JsonDeserializationContext context) {
        if (json == null) {
            return null;
        } else {
            return context.deserialize(json, tClass);
        }
    }

    @Override
    public DataMapBuilder deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext context) throws JsonParseException {
        JsonObject object = jsonElement.getAsJsonObject();

        DataMapType dataMapType = context.deserialize(object.get("type"), DataMapType.class);

        Class<?> parametersClass;
        DataMapParameters parameters = null;
        try {

            if (object.get("ParametersClassName") != null) {
                parametersClass = Class.forName(object.get("ParametersClassName").getAsString());
                parameters = context.deserialize(object.get("parameters"), parametersClass);
            }
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

//        DataMapParameters parameters = context.deserialize(object.get("parameters"), parametersClass);

        return new DataMapBuilder(dataMapType).setParameters(parameters);
    }

    private String getClassName(Object object) {
        if (object == null) {
            return null;
        } else {
            return object.getClass().getName();
        }
    }

    @Override
    public JsonElement serialize(DataMapBuilder dataMapBuilder, Type type, JsonSerializationContext context) {
        JsonObject object = new JsonObject();
//        object.addProperty("ParametersClassName", dataMapBuilder.parameters.getClass().getName());
        object.addProperty("ParametersClassName", getClassName(dataMapBuilder.parameters));
        object.add("type", context.serialize(dataMapBuilder.type));
        object.add("parameters", context.serialize(dataMapBuilder.parameters));
        return object;
    }
}
