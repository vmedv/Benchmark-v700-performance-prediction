//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_KEY_GENERATOR_BUILDER_H
#define SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_KEY_GENERATOR_BUILDER_H

#include "plaf.h"
#include "common.h"

template<typename K>
struct TemporaryWaveSpecialRemoveKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
    TemporaryWaveSpecialRemoveParameters *parameters;

    TemporaryWaveSpecialRemoveKeyGeneratorBuilder(TemporaryWaveSpecialRemoveParameters *_parameters)
            : KeyGeneratorBuilder<K>(_parameters), parameters(_parameters) {
        this->keyGeneratorType = KeyGeneratorType::TEMPORARY_SKEWED;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        KeyGeneratorData<K> *dataMap = new KeyGeneratorData<K>(parameters);

        #pragma omp parallel for
        for (size_t i = 0; i < MAX_THREADS_POW2; ++i) {
            keygens[i] = new TemporaryWaveSpecialRemoveKeyGenerator<K>(
                    parameters,
                    dataMap,
                    parameters->waveDistBuilder->getDistribution(&rngs[i], parameters->MAXKEY),
                    parameters->removeDistBuilder->getDistribution(&rngs[i], parameters->MAXKEY),
                    parameters->prefillDistBuilder->getDistribution(&rngs[i], parameters->MAXKEY)
            );
        }

        return keygens;
    }

};


#endif //SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_KEY_GENERATOR_BUILDER_H
