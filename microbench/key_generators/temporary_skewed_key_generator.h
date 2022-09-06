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


class TemporarySkewedKeyGeneratorData : public KeyGeneratorData {
public:
    PAD;
    size_t maxKey;

    int *data;
    size_t *setLengths;
    size_t *setBegins;
    TemporarySkewedParameters *TSParm;
    PAD;

    TemporarySkewedKeyGeneratorData(const size_t _maxKey, TemporarySkewedParameters *_TSParm) {
        TSParm = _TSParm;
        maxKey = _maxKey;
        data = new int[maxKey];
        for (int i = 0; i < maxKey; i++) {
            data[i] = i + 1;
        }

        std::random_shuffle(data, data + maxKey - 1);

        setLengths = new size_t[TSParm->setCount + 1];
        setBegins = new size_t[TSParm->setCount + 1];
        setBegins[0] = 0;
        for (int i = 0; i < TSParm->setCount; i++) {
            setLengths[i] = (size_t) (maxKey * TSParm->setSizes[i]) + 1;
            setBegins[i + 1] = setBegins[i] + setLengths[i];
        }
    }

    ~TemporarySkewedKeyGeneratorData() {
        delete[] data;
        delete[] setLengths;
        delete[] setBegins;
    }
};

template<typename K>
class TemporarySkewedKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    TemporarySkewedKeyGeneratorData *keygenData;
    Distribution **hotDists;
    Distribution * relaxDist;
    long long time;
    size_t pointer;
    bool relaxTime;
    PAD;

    void update_pointer() {
        if (!relaxTime) {
            if (time > keygenData->TSParm->hotTimes[pointer]) {
                time = -1;
                relaxTime = true;
            }
        } else {
            if (time > keygenData->TSParm->relaxTimes[pointer]) {
                time = -1;
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
    TemporarySkewedKeyGenerator(TemporarySkewedKeyGeneratorData *_keygenData,
                                Distribution *_relaxDist, Distribution **_hotDists)
            : keygenData(_keygenData), relaxDist(_relaxDist), hotDists(_hotDists) {
        time = 0;
        pointer = 0;
        relaxTime = false;
    }

    K next_read() {
        update_pointer();
        K value;
        if (relaxTime) {
            value = keygenData->data[relaxDist->next()];
        } else {
            value = keygenData->data[keygenData->setBegins[pointer] + hotDists[pointer]->next()];
        }
        return value;
    }

    K next_write() {
        update_pointer();
        K value;
        if (relaxTime) {
            value = keygenData->data[relaxDist->next()];
        } else {
            value = keygenData->data[keygenData->setBegins[pointer] + hotDists[pointer]->next()];
        }
        return value;
    }

    K next_erase() {
        return this->next_write();
    }

    K next_insert() {
        return this->next_write();
    }

    K next_prefill() {
        return keygenData->data[relaxDist->next()];
    }
};


#endif //SETBENCH_TEMPORARY_SKEWED_KEY_GENERATOR_H
