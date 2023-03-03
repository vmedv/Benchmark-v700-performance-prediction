//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H
#define SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"


template<typename K>
class TemporarySkewedKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    TemporarySkewedKeyGeneratorData<K> *data;
    Distribution **hotDists;
    Distribution *relaxDist;
    size_t time;
    size_t pointer;
    bool relaxTime;
    bool isLeftRange = true;
    PAD;

    void update_pointer() {
        if (!relaxTime) {
            if (time >= data->parameters->hotTimes[pointer]) {
                time = 0;
                relaxTime = true;
            }
        } else {
            if (time >= data->parameters->relaxTimes[pointer]) {
                time = 0;
                relaxTime = false;
                ++pointer;
                if (pointer >= data->parameters->setCount) {
                    pointer = 0;
                }
            }
        }
        ++time;
    }

public:
    TemporarySkewedKeyGenerator(TemporarySkewedKeyGeneratorData<K> *_data,
                                Distribution *_relaxDist, Distribution **_hotDists)
            : data(_data), relaxDist(_relaxDist), hotDists(_hotDists) {
        time = 0;
        pointer = 0;
        relaxTime = false;
    }

    K next() {
        update_pointer();
        K value;

        if (relaxTime) {
            value = data->get(relaxDist->next());
        } else {
            size_t index = data->setBegins[pointer] + hotDists[pointer]->next();
            if (index >= data->maxKey) {
                index -= data->maxKey;
            }

            value = data->get(index);
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

    K next_range() {
        K value = next();
        if (isLeftRange) {
            time--;
        }
        isLeftRange = !isLeftRange;
        return value;
    }

    K next_prefill() {
        return data->get(relaxDist->next());
    }

    ~TemporarySkewedKeyGenerator() {
        // todo delete data
        delete relaxDist;
        delete[] hotDists;
    }
};


#endif //SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H
