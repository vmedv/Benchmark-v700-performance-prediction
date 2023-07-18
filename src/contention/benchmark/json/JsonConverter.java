package contention.benchmark.json;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import contention.benchmark.Parameters;
import contention.benchmark.workload.ThreadLoops.abstractions.ThreadLoopBuilderOld;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilderOld;
import contention.benchmark.workload.distributions.abstractions.DistributionBuilderOld;
import contention.benchmark.json.converters.*;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilderOld;
import contention.benchmark.stop.condition.StopCondition;

public class JsonConverter {
    private static final Gson gson = new GsonBuilder()
            .registerTypeAdapter(ThreadLoopBuilderOld.class, new ThreadLoopBuilderConverter())
            .registerTypeAdapter(KeyGeneratorBuilderOld.class, new KeyGeneratorBuilderConverter())
            .registerTypeAdapter(DistributionBuilderOld.class, new DistributionBuilderConverter())
            .registerTypeAdapter(DataMapBuilderOld.class, new DataMapBuilderConverter())
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
