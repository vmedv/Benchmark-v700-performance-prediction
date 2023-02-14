//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_SIMPLE_KEY_GENERATOR_BUILDER_H
#define SETBENCH_SIMPLE_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "key_generators/key_generator.h"
#include "key_generators/simple_key_generator.h"
#include "parameters/simple_parameters.h"

#include "distributions/distribution.h"
#include "distributions/mutable_zipf_distribution.h"
#include "distributions/skewed_sets_distribution.h"
#include "distributions/uniform_distribution.h"
#include "distributions/zipf_distribution.h"
#include "distributions/zipf_rejection_inversion_sampler.h"

template<typename K>
class SimpleKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
public:
    SimpleParameters *SParm;

    SimpleKeyGeneratorBuilder(SimpleParameters *_SParm) : SParm(_SParm) {
        this->keyGeneratorType = KeyGeneratorType::SIMPLE_KEYGEN;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K>*[MAX_THREADS_POW2];

        switch (SParm->distributionType) {

            case DistributionType::ZIPF: {
                ZipfDistributionData *zipfKeygenData =
                        new ZipfDistributionData(maxkeyToGenerate, SParm->zipf_parm);

#pragma omp parallel for
                for (int i = 0; i < MAX_THREADS_POW2; ++i) {
                    keygens[i] = new SimpleKeyGenerator<K>(
                            new ZipfDistribution(zipfKeygenData, &rngs[i]));
                }
            }
                break;
            case DistributionType::ZIPF_FAST: {
                ZipfRejectionInversionSamplerData *zipfFastKeygenData =
                        new ZipfRejectionInversionSamplerData((int) maxkeyToGenerate);

#pragma omp parallel for
                for (int i = 0; i < MAX_THREADS_POW2; ++i) {
                    keygens[i] = new SimpleKeyGenerator<K>(
                            new ZipfRejectionInversionSampler(
                                    zipfFastKeygenData, SParm->zipf_parm, &rngs[i]));
                }
            }
                break;
            case DistributionType::UNIFORM: {
#pragma omp parallel for
                for (int i = 0; i < MAX_THREADS_POW2; ++i) {
                    keygens[i] = new SimpleKeyGenerator<K>(
                            new UniformDistribution(&rngs[i], maxkeyToGenerate));
                }
            }
                break;
            default: {
                setbench_error("invalid case");
            }
        }

        return keygens;
    }

};

#endif //SETBENCH_SIMPLE_KEY_GENERATOR_BUILDER_H
