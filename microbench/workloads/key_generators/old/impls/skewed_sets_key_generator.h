//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_SKEWED_SETS_KEY_GENERATOR_H
#define SETBENCH_SKEWED_SETS_KEY_GENERATOR_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"


template<typename K>
class SkewedSetsKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    SkewedSetsKeyGeneratorData<K> *data;
    Random64 *rng;
    Distribution *readDist;
    Distribution *writeDist;
    size_t prefillSize;
    bool writePrefillOnly;
    PAD;
public:
    SkewedSetsKeyGenerator(SkewedSetsKeyGeneratorData<K> *_data, Random64 *_rng,
                           Distribution *_readDist, Distribution *_writeDist, bool _writePrefillOnly)
            : data(_data), rng(_rng), readDist(_readDist), writeDist(_writeDist),
              writePrefillOnly(_writePrefillOnly) {}

    K next_read() {
        return data->get(readDist->next());
    }

    K next_write() {
        return data->get((data->writeSetBegin + writeDist->next()) % data->maxKey);
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_range() {
        return this->next_read();
    }

    K next_prefill() {
        K value = 0;
        if (writePrefillOnly) {
            value = next_write();
        }
        else {
            if (prefillSize < data->readSetLength) {
                value = data->get(prefillSize++);
            } else {
                value = data->get(data->readSetLength +
                                  rng->next(data->maxKey - data->readSetLength));
            }
        }
        return value;
    }

    ~SkewedSetsKeyGenerator() {
//        todo delete keygenData;
        delete readDist;
        delete writeDist;
    }
};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_H
