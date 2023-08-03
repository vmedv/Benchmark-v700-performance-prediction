package contention.benchmark.workload.key.generators.builders;

import contention.benchmark.workload.key.generators.abstractions.KeyGenerator;
import contention.benchmark.workload.key.generators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.key.generators.impls.CreakersAndWavePrefillKeyGenerator;
import contention.benchmark.workload.key.generators.parameters.CreakersAndWaveParameters;

public class CreakersAndWavePrefillKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public CreakersAndWaveParameters parameters;

    public CreakersAndWavePrefillKeyGeneratorBuilder(CreakersAndWaveParameters parameters) {
        this.parameters = parameters;
    }

    public CreakersAndWavePrefillKeyGeneratorBuilder setParameters(CreakersAndWaveParameters parameters) {
        this.parameters = parameters;
        return this;
    }
    @Override
    public KeyGeneratorBuilder init(int range) {
        parameters.init(range);
        return this;
    }

    @Override
    public KeyGenerator build() {
        return new CreakersAndWavePrefillKeyGenerator(parameters.dataMapBuilder.getOrBuild(), parameters);
    }

    @Override
    public StringBuilder toStringBuilder() {
        //TODO: toStringBuilder
        return new StringBuilder();
    }
}
