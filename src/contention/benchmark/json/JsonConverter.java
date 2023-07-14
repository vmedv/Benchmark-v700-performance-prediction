package contention.benchmark.json;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import contention.benchmark.Parameters;
import contention.benchmark.ThreadLoops.abstractions.ThreadLoopBuilder;
import contention.benchmark.datamap.abstractions.DataMapBuilder;
import contention.benchmark.distributions.abstractions.DistributionBuilder;
import contention.benchmark.json.converters.*;
import contention.benchmark.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.stop.condition.StopCondition;

public class JsonConverter {
    private static final Gson gson = new GsonBuilder()
            .registerTypeAdapter(ThreadLoopBuilder.class, new ThreadLoopBuilderConverter())
            .registerTypeAdapter(KeyGeneratorBuilder.class, new KeyGeneratorBuilderConverter())
            .registerTypeAdapter(DistributionBuilder.class, new DistributionBuilderConverter())
            .registerTypeAdapter(DataMapBuilder.class, new DataMapBuilderConverter())
            .registerTypeAdapter(StopCondition.class, new StopConditionConverter())
            .setPrettyPrinting()
            .create();

    public static String toJson(Object object) {
        return gson.toJson(object);
    }

    public static Parameters fromJson(String json) {
        return gson.fromJson(json, Parameters.class);
    }

    public static <T> T fromJsonT(String json, Class<T> tClass) {
        return gson.fromJson(json, tClass);
    }
}
