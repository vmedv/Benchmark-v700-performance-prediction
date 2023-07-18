package contention.benchmark.workload.keygenerators.keygenerators.builders;

import contention.benchmark.workload.distributions.abstractions.DistributionBuilder;
import contention.benchmark.workload.distributions.abstractions.MutableDistributionBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGeneratorBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.parameters.CreakersAndWaveParameters;
import contention.benchmark.workload.datamap.abstractions.DataMapBuilder;
import contention.benchmark.workload.keygenerators.keygenerators.abstractions.KeyGenerator;
import contention.benchmark.workload.keygenerators.keygenerators.impls.CreakersAndWaveKeyGenerator;

public class CreakersAndWaveKeyGeneratorBuilder implements KeyGeneratorBuilder {
    public CreakersAndWaveParameters parameters = new CreakersAndWaveParameters();

    public CreakersAndWaveKeyGeneratorBuilder setParameters(CreakersAndWaveParameters parameters) {
        this.parameters = parameters;
        return this;
    }

    public CreakersAndWaveKeyGeneratorBuilder setCreakersSize(double creakersSize) {
        parameters.creakersSize = creakersSize;
        return this;
    }

    public CreakersAndWaveKeyGeneratorBuilder setCreakersProb(double creakersProb) {
        parameters.creakersProb = creakersProb;
        return this;
    }

    public CreakersAndWaveKeyGeneratorBuilder setWaveSize(double waveSize) {
        parameters.waveSize = waveSize;
        return this;
    }

    public CreakersAndWaveKeyGeneratorBuilder setCreakersDistBuilder(DistributionBuilder creakersDistBuilder) {
        parameters.creakersDistBuilder = creakersDistBuilder;
        return this;
    }

    public CreakersAndWaveKeyGeneratorBuilder setWaveDistBuilder(MutableDistributionBuilder waveDistBuilder) {
        parameters.waveDistBuilder = waveDistBuilder;
        return this;
    }

    public CreakersAndWaveKeyGeneratorBuilder setDataMapBuilder(DataMapBuilder dataMapBuilder) {
        parameters.dataMapBuilder = dataMapBuilder;
        return this;
    }


    @Override
    public KeyGeneratorBuilder init(int range) {
        parameters.init(range);
        return this;
    }

    @Override
    public KeyGenerator build() {
        return new CreakersAndWaveKeyGenerator(
                parameters.dataMapBuilder.getOrBuild(),
                parameters
        );
    }

    @Override
    public StringBuilder toStringBuilder() {
        return parameters.toStringBuilder();
    }
}
