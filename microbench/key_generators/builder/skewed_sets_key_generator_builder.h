//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_SKEWED_SETS_KEY_GENERATOR_BUILDER_H
#define SETBENCH_SKEWED_SETS_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "common.h"

template<typename K>
struct SkewedSetsKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
    PAD;
    SkewedSetsParameters *parameters;
    PAD;

    SkewedSetsKeyGeneratorBuilder(SkewedSetsParameters *_parameters)
            : KeyGeneratorBuilder<K>(_parameters), parameters(_parameters) {
        this->keyGeneratorType = KeyGeneratorType::SKEWED_SETS;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        SkewedSetsKeyGeneratorData<K> *skewedSetsKeygenData =
                new SkewedSetsKeyGeneratorData<K>(parameters);

#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            size_t readHotSize = (size_t) (maxkeyToGenerate * parameters->READ_HOT_SIZE);
            size_t writeHotSize = (size_t) (maxkeyToGenerate * parameters->WRITE_HOT_SIZE);

            keygens[i] = new SkewedSetsKeyGenerator<K>(
                    skewedSetsKeygenData, &rngs[i],
                    new SkewedUniformDistribution(
                            new UniformDistribution(&rngs[i], readHotSize),
                            new UniformDistribution(&rngs[i], maxkeyToGenerate - readHotSize),
                            &rngs[i], parameters->READ_HOT_PROB, readHotSize),
                    new SkewedUniformDistribution(
                            new UniformDistribution(&rngs[i], writeHotSize),
                            new UniformDistribution(&rngs[i], maxkeyToGenerate - writeHotSize),
                            &rngs[i], parameters->WRITE_HOT_PROB, writeHotSize),
                    parameters->writePrefillOnly
            );
        }

        return keygens;
    }

};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_BUILDER_H
