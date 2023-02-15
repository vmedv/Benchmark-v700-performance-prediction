//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H
#define SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "key_generator.h"
#include "distributions/distribution.h"
#include "parameters/temporary_skewed_parameters.h"
#include "data/temporary_skewed_key_generator_data.h"


template<typename K>
class TemporarySkewedKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    TemporarySkewedKeyGeneratorData<K> *keygenData;
    Distribution **hotDists;
    Distribution *relaxDist;
    size_t time;
    size_t pointer;
    bool relaxTime;
    bool isLeftRange = true;
    PAD;

    void update_pointer() {
        if (!relaxTime) {
            if (time >= keygenData->TSParm->hotTimes[pointer]) {
                time = 0;
                relaxTime = true;
            }
        } else {
            if (time >= keygenData->TSParm->relaxTimes[pointer]) {
                time = 0;
                relaxTime = false;
                ++pointer;
                if (pointer >= keygenData->TSParm->setCount) {
                    pointer = 0;
                }
            }
        }
        ++time;
    }

public:
    TemporarySkewedKeyGenerator(TemporarySkewedKeyGeneratorData<K> *_keygenData,
                                Distribution *_relaxDist, Distribution **_hotDists)
            : keygenData(_keygenData), relaxDist(_relaxDist), hotDists(_hotDists) {
        time = 0;
        pointer = 0;
        relaxTime = false;
    }

    K next() {
        update_pointer();
        K value;

        if (relaxTime) {
            value = keygenData->get(relaxDist->next());
        } else {
            size_t index = keygenData->setBegins[pointer] + hotDists[pointer]->next();
            if (index >= keygenData->maxKey) {
                index -= keygenData->maxKey;
            }

            value = keygenData->get(index);
        }

        return value;
    }

    K next_read() {
        return next();
    }

    K next_write() {
        return next();
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_prefill() {
        return keygenData->get(relaxDist->next());
    }


    K next_range() {
        K value = next();
        if (isLeftRange) {
            time--;
        }
        isLeftRange = !isLeftRange;
        return value;
    }

};


#endif //SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H
