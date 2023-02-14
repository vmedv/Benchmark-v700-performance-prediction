//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_BUILDER_H
#define SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "key_generators/key_generator.h"
#include "key_generators/creakers_and_wave_key_generator.h"
#include "distributions/distribution.h"
#include "distributions/mutable_zipf_distribution.h"
#include "distributions/skewed_sets_distribution.h"
#include "distributions/uniform_distribution.h"
#include "distributions/zipf_distribution.h"
#include "distributions/zipf_rejection_inversion_sampler.h"
#include "key_generators/data/creakers_and_wave_key_generator_data.h"

template<typename K>
class CreakersAndWaveKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
public:
    CreakersAndWaveParameters *CWParm;

    CreakersAndWaveKeyGeneratorBuilder(CreakersAndWaveParameters *_CWParm) : CWParm(_CWParm) {
        this->keyGeneratorType = KeyGeneratorType::CREAKERS_AND_WAVE;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K>*[MAX_THREADS_POW2];

        CreakersAndWaveKeyGeneratorData<K> *creakersAndWaveKeygenData =
                new CreakersAndWaveKeyGeneratorData<K>(maxkeyToGenerate, CWParm);

#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            keygens[i] = new CreakersAndWaveKeyGenerator<K>(
                    creakersAndWaveKeygenData, &rngs[i],
                    new UniformDistribution(&rngs[i], creakersAndWaveKeygenData->creakersLength),
                    new MutableZipfDistribution(&rngs[i], CWParm->waveZipfParm)
            );
        }

        return keygens;
    }
};

#endif //SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_BUILDER_H
