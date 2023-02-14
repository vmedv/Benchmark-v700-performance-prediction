//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_NON_SHUFFLE_DATA_H
#define SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_NON_SHUFFLE_DATA_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "key_generators/key_generator.h"
#include "distributions/distribution.h"
#include "parameters/temporary_skewed_parameters.h"
#include "temporary_skewed_key_generator_data.h"

template<typename K>
class TemporarySkewedKeyGeneratorNonShuffleData : public TemporarySkewedKeyGeneratorData<K> {
public:
    PAD;
//    size_t maxKey;

//    size_t *setLengths;
//    size_t *setBegins;
//    TemporarySkewedParameters *TSParm;
    PAD;

    TemporarySkewedKeyGeneratorNonShuffleData(const size_t _maxKey, TemporarySkewedParameters *_TSParm)
//            : this->maxKey(_maxKey) {
    {
        this->maxKey = _maxKey;
        this->TSParm = _TSParm;
        this->setLengths = new size_t[TSParm->setCount];
        this->setBegins = new size_t[TSParm->setCount];
        for (int i = 0; i < TSParm->setCount; i++) {
            this->setLengths[i] = (size_t) (this->maxKey * TSParm->setSizes[i]);
            this->setBegins[i] = (size_t) (this->maxKey * TSParm->setBegins[i]);
//            setBegins[i + 1] = setBegins[i] + setLengths[i];
        }
    }

    K get(size_t index) {
        return index + 1;
    }

    ~TemporarySkewedKeyGeneratorNonShuffleData() {
        delete[] this->setLengths;
        delete[] this->setBegins;
    }
};


#endif //SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_NON_SHUFFLE_DATA_H
