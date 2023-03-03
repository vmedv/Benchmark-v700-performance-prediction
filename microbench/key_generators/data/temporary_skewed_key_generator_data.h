//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_DATA_H
#define SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_DATA_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"

template<typename K>
struct TemporarySkewedKeyGeneratorData : public KeyGeneratorData<K> {
    PAD;
    size_t maxKey;

    size_t *setLengths;
    size_t *setBegins;
    TemporarySkewedParameters *parameters;
    PAD;

    explicit TemporarySkewedKeyGeneratorData() {}

    TemporarySkewedKeyGeneratorData(TemporarySkewedParameters *_parameters)
            : KeyGeneratorData<K>(_parameters), maxKey(_parameters->MAXKEY), parameters(_parameters) {

        setLengths = new size_t[parameters->setCount];
        setBegins = new size_t[parameters->setCount];

        for (int i = 0; i < parameters->setCount; i++) {
            setLengths[i] = (size_t) (maxKey * _parameters->setSizes[i]);
        }

        if (!parameters->isNonShuffle) {
            setBegins[0] = 0;
            for (int i = 1; i < parameters->setCount; i++) {
                setBegins[i] = setBegins[i - 1] + setLengths[i - 1];
            }
        } else {
            for (int i = 0; i < parameters->setCount; i++) {
                setBegins[i] = (size_t) (maxKey * parameters->setBegins[i]);
            }
        }
    }

    ~TemporarySkewedKeyGeneratorData() {
        delete[] setLengths;
        delete[] setBegins;
    }
};


#endif //SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_DATA_H
