//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_OPS_KEY_GENERATOR_BUILDER_H
#define SETBENCH_OPS_KEY_GENERATOR_BUILDER_H

#include "errors.h"
#include "plaf.h"
#include "common.h"

template<typename K>
struct OpsKeyGeneratorBuilder : public KeyGeneratorBuilder<K> {
    OpsParameters *parameters;

    OpsKeyGeneratorBuilder(OpsParameters *_parameters)
            : KeyGeneratorBuilder<K>(_parameters), parameters(_parameters) {
        this->keyGeneratorType = KeyGeneratorType::OPS;
    }

    KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 *rngs) {

        KeyGenerator<K> **keygens = new KeyGenerator<K> *[MAX_THREADS_POW2];

        KeyGeneratorData<K> *data = new KeyGeneratorData<K>(parameters);


#pragma omp parallel for
        for (int i = 0; i < MAX_THREADS_POW2; ++i) {
            Distribution *prefillDistribution = parameters->prefill_sequential ? nullptr :
                                                (new DistributionBuilder())->getDistribution(
                                                        &rngs[i], maxkeyToGenerate);

            keygens[i] = new OpsKeyGenerator<K>(
                    data,
                    parameters->readDistBuilder->getDistribution(&rngs[i], maxkeyToGenerate),
                    parameters->insertDistBuilder->getDistribution(&rngs[i], maxkeyToGenerate),
                    parameters->removeDistBuilder->getDistribution(&rngs[i], maxkeyToGenerate),
                    prefillDistribution,
                    parameters->prefill_sequential
            );
        }

        return keygens;
    }

};

#endif //SETBENCH_OPS_KEY_GENERATOR_BUILDER_H
