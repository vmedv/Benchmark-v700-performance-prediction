//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_SKEWED_SETS_KEY_GENERATOR_DATA_H
#define SETBENCH_SKEWED_SETS_KEY_GENERATOR_DATA_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "key_generators/key_generator.h"
#include "distributions/distribution.h"
#include "parameters/skewed_sets_parameters.h"


template<typename K>
class SkewedSetsKeyGeneratorData : public KeyGeneratorData<K> {
public:
    PAD;
    size_t maxKey;
    size_t readSetLength;
    size_t writeSetLength;

    size_t writeSetBegin;
    size_t writeSetEnd;

    int *data;
    PAD;

    SkewedSetsKeyGeneratorData(const size_t _maxKey,
                               const double _readSetSize, const double _writeSetSize, const double _interSetSize) {
        maxKey = _maxKey;
        data = new int[maxKey];
        for (int i = 0; i < maxKey; i++) {
            data[i] = i + 1;
        }

        std::random_shuffle(data, data + maxKey - 1);

        readSetLength = (size_t) (maxKey * _readSetSize);
        writeSetLength = (size_t) (maxKey * _writeSetSize);
        size_t interSetLength = (size_t) (maxKey * _interSetSize);

        writeSetBegin = readSetLength - interSetLength;
        writeSetEnd = writeSetBegin + writeSetLength;
    }

    K get(size_t index) {
        return data[index];
    }

    ~SkewedSetsKeyGeneratorData() {
        delete[] data;
    }
};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_DATA_H
