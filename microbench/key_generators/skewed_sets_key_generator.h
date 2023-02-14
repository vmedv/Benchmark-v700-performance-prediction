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
#include "data/skewed_sets_key_generator_data.h"


template<typename K>
class SkewedSetsKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    SkewedSetsKeyGeneratorData<K> *keygenData;
    Random64 *rng;
    Distribution *readDist;
    Distribution *writeDist;
    size_t prefillSize;
    bool writePrefillOnly;
    PAD;
public:
    SkewedSetsKeyGenerator(SkewedSetsKeyGeneratorData<K> *_keygenData, Random64 *_rng,
                           Distribution *_readDist, Distribution *_writeDist, bool _writePrefillOnly)
            : keygenData(_keygenData), rng(_rng), readDist(_readDist), writeDist(_writeDist),
              writePrefillOnly(_writePrefillOnly) {}

    K next_read() {
        return keygenData->get(readDist->next());
    }

    K next_write() {
        return keygenData->get((keygenData->writeSetBegin + writeDist->next()) % keygenData->maxKey);
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_prefill() {
        K value = 0;
        if (writePrefillOnly) {
            value = next_write();
        }
        else {
            if (prefillSize < keygenData->readSetLength) {
                value = keygenData->get(prefillSize++);
            } else {
                value = keygenData->get(keygenData->readSetLength +
                                         rng->next(keygenData->maxKey - keygenData->readSetLength));
            }
        }
        return value;
    }
};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_H
