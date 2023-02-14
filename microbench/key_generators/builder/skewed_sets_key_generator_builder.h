//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_SKEWED_SETS_KEY_GENERATOR_BUILDER_H
#define SETBENCH_SKEWED_SETS_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "key_generators/key_generator.h"
#include "key_generators/skewed_sets_key_generator.h"
#include "key_generators/data/skewed_sets_key_generator_data.h"
#include "distributions/distribution.h"
#include "distributions/skewed_sets_distribution.h"
#include "distributions/uniform_distribution.h"

template<typename K>
class SkewedSetsKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
public:
    PAD;
    SkewedSetsParameters *SkeSParm;
    PAD;

    SkewedSetsKeyGeneratorBuilder(SkewedSetsParameters *SkeSParm) : SkeSParm(SkeSParm) {
        this->keyGeneratorType = KeyGeneratorType::SKEWED_SETS;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        SkewedSetsKeyGeneratorData<K> *skewedSetsKeygenData =
                new SkewedSetsKeyGeneratorData<K>(maxkeyToGenerate,
                                                  SkeSParm->READ_HOT_SIZE,
                                                  SkeSParm->WRITE_HOT_SIZE,
                                                  SkeSParm->INTERSECTION
                );

#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            size_t readHotSize = (size_t) (maxkeyToGenerate * SkeSParm->READ_HOT_SIZE);
            size_t writeHotSize = (size_t) (maxkeyToGenerate * SkeSParm->WRITE_HOT_SIZE);

            keygens[i] = new SkewedSetsKeyGenerator<K>(
                    skewedSetsKeygenData, &rngs[i],
                    new SkewedSetsDistribution(
                            new UniformDistribution(&rngs[i], readHotSize),
                            new UniformDistribution(&rngs[i], maxkeyToGenerate - readHotSize),
                            &rngs[i], SkeSParm->READ_HOT_PROB, readHotSize),
                    new SkewedSetsDistribution(
                            new UniformDistribution(&rngs[i], writeHotSize),
                            new UniformDistribution(&rngs[i], maxkeyToGenerate - writeHotSize),
                            &rngs[i], SkeSParm->WRITE_HOT_PROB, writeHotSize),
                    SkeSParm->writePrefillOnly
            );
        }

        return keygens;
    }

};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_BUILDER_H
