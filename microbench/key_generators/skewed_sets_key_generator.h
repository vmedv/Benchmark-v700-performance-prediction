//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_SKEWED_SETS_KEY_GENERATOR_H
#define SETBENCH_SKEWED_SETS_KEY_GENERATOR_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "key_generator.h"
#include "distributions/distribution.h"
#include "parameters/skewed_sets_parameters.h"

class SkewedSetsKeyGeneratorData : public KeyGeneratorData {
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

    ~SkewedSetsKeyGeneratorData() {
        delete[] data;
    }
};

template<typename K>
class SkewedSetsKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    SkewedSetsKeyGeneratorData *keygenData;
    Random64 *rng;
    Distribution *readDist;
    Distribution *writeDist;
    size_t prefillSize;
    PAD;
public:
    SkewedSetsKeyGenerator(SkewedSetsKeyGeneratorData *_keygenData, Random64 *_rng,
                           Distribution *_readDist, Distribution *_writeDist)
            : keygenData(_keygenData), rng(_rng), readDist(_readDist), writeDist(_writeDist) {}

    K next_read() {
        return keygenData->data[readDist->next()];
    }

    K next_write() {
        return keygenData->data[(keygenData->writeSetBegin + writeDist->next()) % keygenData->maxKey];
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_prefill() {
        K value = 0;
        if (prefillSize < keygenData->readSetLength) {
            value = keygenData->data[prefillSize++];
        } else {
            value = keygenData->data[keygenData->readSetLength +
                                     rng->next(keygenData->maxKey - keygenData->readSetLength)];
        }
        return value;
    }
};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_H
