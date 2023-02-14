//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_BUILDER_H
#define SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_BUILDER_H

#include "plaf.h"
#include "key_generators/key_generator.h"
#include "key_generators/temporary_skewed_key_generator.h"
#include "key_generators/data/temporary_skewed_key_generator_data.h"
#include "key_generators/data/temporary_skewed_key_generator_non_shuffle_data.h"
#include "distributions/distribution.h"
#include "distributions/skewed_sets_distribution.h"
#include "distributions/uniform_distribution.h"

template<typename K>
class TemporarySkewedKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
public:
    TemporarySkewedParameters *TSParm;

    TemporarySkewedKeyGeneratorBuilder(TemporarySkewedParameters *TSParm) : TSParm(TSParm) {
        this->keyGeneratorType = KeyGeneratorType::TEMPORARY_SKEWED;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        TemporarySkewedKeyGeneratorData<K> *temporarySkewedKeygenData;

        if (TSParm->isNotShuffle) {
            temporarySkewedKeygenData = new TemporarySkewedKeyGeneratorNonShuffleData<K>(maxkeyToGenerate, TSParm);
        } else {
            temporarySkewedKeygenData = new TemporarySkewedKeyGeneratorData<K>(maxkeyToGenerate, TSParm);
        }

#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            Distribution **hotDists = new Distribution *[TSParm->setCount];
            //static_cast<Distribution<test_type> **>(malloc(TSParm->setCount * sizeof(void *)));
            for (int j = 0; j < TSParm->setCount; ++j) {
                size_t setSize = (size_t) (maxkeyToGenerate * TSParm->setSizes[j]);

                hotDists[j] =
                        new SkewedSetsDistribution(
                                new UniformDistribution(&rngs[i], setSize),
                                new UniformDistribution(&rngs[i], maxkeyToGenerate - setSize),
                                &rngs[i], TSParm->hotProbs[j], setSize
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
