//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_KEY_GENERATOR_H
#define SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_KEY_GENERATOR_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"


template<typename K>
class TemporaryWaveSpecialRemoveKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    KeyGeneratorData<K> *data;
    Distribution *waveDist;
    Distribution *removeDist;
    Distribution *prefillDist;
    TemporaryWaveSpecialRemoveParameters *parameters;
    size_t *setBegins;
    size_t time;
    size_t pointer;
    bool isLeftRange = true;
    PAD;

    void update_pointer() {
        if (time >= parameters->hotTimes[pointer]) {
            time = 0;
            ++pointer;
            if (pointer >= parameters->setCount) {
                pointer = 0;
            }
        }
        ++time;
    }

public:
    TemporaryWaveSpecialRemoveKeyGenerator(TemporaryWaveSpecialRemoveParameters *_parameters,
                                           KeyGeneratorData<K> *_data,
                                           Distribution *_waveDist, Distribution *_removeDist,
                                           Distribution *_prefillDist)
            : parameters(_parameters), data(_data),
              waveDist(_waveDist), removeDist(_removeDist), prefillDist(_prefillDist) {
        time = 0;
        pointer = 0;

        setBegins = new size_t[parameters->setCount];
        for (size_t i = 0; i < parameters->setCount; i++) {
            setBegins[i] = (size_t) (parameters->MAXKEY * parameters->setBegins[i]);
        }
    }

    K next() {
        update_pointer();
        return data->get((setBegins[pointer] + waveDist->next()) % parameters->MAXKEY);
    }

    K next_read() {
        return next();
    }

    K next_insert() {
        return next();
    }

    K next_erase() {
        update_pointer();
        return data->get(removeDist->next());
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
        return data->get(prefillDist->next());
    }

    ~TemporaryWaveSpecialRemoveKeyGenerator() {
        delete setBegins;
        delete waveDist;
        delete removeDist;
        delete prefillDist;
    }
};


#endif //SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_KEY_GENERATOR_H


