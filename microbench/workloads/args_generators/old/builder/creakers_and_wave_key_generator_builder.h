//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_BUILDER_H
#define SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "common.h"

template<typename K>
struct CreakersAndWaveKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
    CreakersAndWaveParameters *parameters;

    CreakersAndWaveKeyGeneratorBuilder(CreakersAndWaveParameters *_parameters)
            : KeyGeneratorBuilder<K>(_parameters), parameters(_parameters) {
        this->keyGeneratorType = KeyGeneratorType::CREAKERS_AND_WAVE;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        CreakersAndWaveKeyGeneratorData<K> *creakersAndWaveKeygenData =
                new CreakersAndWaveKeyGeneratorData<K>(parameters);

#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            keygens[i] = new CreakersAndWaveKeyGenerator<K>(
                    creakersAndWaveKeygenData, &rngs[i],
                    parameters->creakersDistBuilder->
                            getDistribution(&rngs[i], creakersAndWaveKeygenData->creakersLength),
                    parameters->waveDistBuilder->getMutableDistribution(&rngs[i])
            );
        }

        return keygens;
    }
};

#endif //SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_BUILDER_H
