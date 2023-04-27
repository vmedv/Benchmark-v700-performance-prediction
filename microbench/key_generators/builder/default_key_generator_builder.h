//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_DEFAULT_KEY_GENERATOR_BUILDER_H
#define SETBENCH_DEFAULT_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "common.h"

template<typename K>
struct DefaultKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
    DefaultParameters *parameters;

    DefaultKeyGeneratorBuilder(DefaultParameters *_parameters)
            : KeyGeneratorBuilder<K>(_parameters), parameters(_parameters) {
        this->keyGeneratorType = KeyGeneratorType::SIMPLE_KEYGEN;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        if (parameters->distributionBuilder->distributionType == DistributionType::UNIFORM) {
            parameters->isNonShuffle = true;
        }

        KeyGeneratorData<K> *data = new KeyGeneratorData<K>(parameters);


#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            Distribution *prefillDistribution = parameters->prefill_sequential ? nullptr :
                                                (new DistributionBuilder())->getDistribution(
                                                        &rngs[i], maxkeyToGenerate);

            keygens[i] = new DefaultKeyGenerator<K>(
                    data,
                    parameters->distributionBuilder->getDistribution(&rngs[i], maxkeyToGenerate),
                    prefillDistribution,
                    parameters->prefill_sequential
            );
        }


        return keygens;
    }

};

#endif //SETBENCH_DEFAULT_KEY_GENERATOR_BUILDER_H
