package contention.benchmark.json;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import contention.benchmark.json.serializers.*;
import contention.benchmark.workload.BenchParameters;
import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.args.generators.abstractions.ArgsGeneratorBuilder;
import contention.benchmark.workload.thread.loops.abstractions.ThreadLoopBuilder;
import contention.benchmark.workload.data.map.abstractions.DataMapBuilder;
import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.stop.condition.StopCondition;

public class JsonConverter {
    private JsonConverter(){}
    private static final Gson gson = new GsonBuilder()
            .registerTypeAdapter(ThreadLoopBuilder.class, new ThreadLoopBuilderSerializer())
            .registerTypeAdapter(ArgsGeneratorBuilder.class, new ArgsGeneratorBuilderSerializer())
            .registerTypeAdapter(DistributionBuilder.class, new DistributionBuilderSerializer())
            .registerTypeAdapter(MutableDistributionBuilder.class, new DistributionBuilderSerializer())
            .registerTypeAdapter(StopCondition.class, new StopConditionSerializer())
            .registerTypeAdapter(DataMapBuilder.class, new DataMapBuilderSerializer())
            .setPrettyPrinting()
            .create();

    public static String toJson(Object object) {
        return gson.toJson(object);
    }

    public static BenchParameters fromJson(String json) {
        return gson.fromJson(json, BenchParameters.class);
    }

    public static <T> T fromJson(String json, Class<T> tClass) {
        return gson.fromJson(json, tClass);
    }
}
