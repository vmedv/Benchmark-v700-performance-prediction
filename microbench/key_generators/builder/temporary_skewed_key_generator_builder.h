//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_BUILDER_H
#define SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_BUILDER_H

#include "plaf.h"
#include "common.h"

template<typename K>
struct TemporarySkewedKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
    TemporarySkewedParameters *parameters;

    TemporarySkewedKeyGeneratorBuilder(TemporarySkewedParameters *_parameters)
            : KeyGeneratorBuilder<K>(_parameters), parameters(_parameters) {
        this->keyGeneratorType = KeyGeneratorType::TEMPORARY_SKEWED;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        TemporarySkewedKeyGeneratorData<K> *temporarySkewedKeygenData =
                new TemporarySkewedKeyGeneratorData<K>(parameters);

#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            Distribution **hotDists = new Distribution *[parameters->setCount];
            //static_cast<Distribution<test_type> **>(malloc(TSParm->setCount * sizeof(void *)));
            for (int j = 0; j < parameters->setCount; ++j) {
                size_t setSize = (size_t) (maxkeyToGenerate * parameters->setSizes[j]);

                hotDists[j] =
                        new SkewedSetsDistribution(
                                new UniformDistribution(&rngs[i], setSize),
                                new UniformDistribution(&rngs[i], maxkeyToGenerate - setSize),
                                &rngs[i], parameters->hotProbs[j], setSize
                        );
            }
            keygens[i] = new TemporarySkewedKeyGenerator<K>(
                    temporarySkewedKeygenData,
                    new UniformDistribution(&rngs[i], maxkeyToGenerate),
                    hotDists
            );
        }

        return keygens;
    }

};


#endif //SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_BUILDER_H
