//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_H
#define SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_H

#include <algorithm>
#include <atomic>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"


template<typename K>
class CreakersAndWaveKeyGenerator : public KeyGenerator<K> {
public:
    PAD;
    CreakersAndWaveKeyGeneratorData<K> *data;
private:
    Random64 *rng;
    Distribution *creakersDist;
    MutableDistribution *waveDist;
    size_t prefillSize;
    PAD;

    /**
     *
     * @return true - creakers, false - wave
     */
    bool next_coin() {
        double z; // Uniform random number (0 < z < 1)

        do {
            z = (rng->next() / (double) rng->max_value);
        } while ((z == 0) || (z == 1));

        return z < data->parameters->CREAKERS_PROB;
    }

public:
    CreakersAndWaveKeyGenerator(CreakersAndWaveKeyGeneratorData<K> *_data, Random64 *_rng,
                                Distribution *_creakersDist, MutableDistribution *_waveDist)
            : data(_data), rng(_rng), creakersDist(_creakersDist), waveDist(_waveDist) {}

    K next_read() {
        K value;
        if (next_coin()) {
            value = data->get(data->creakersBegin + creakersDist->next());
        } else {
            /**
             * In waveDist the first indexes have a higher probability
             */
            size_t localWaveBegin = data->waveBegin;
            size_t localWaveEnd = data->waveEnd;
            size_t localWaveLength =
                    (localWaveEnd - localWaveBegin + data->creakersBegin) % data->creakersBegin;
            size_t index = (localWaveBegin + waveDist->next(localWaveLength)) % data->creakersBegin;
            value = data->get(index);
        }
        return value;
    }

    K next_write() {
        return NULL;
    }

    K next_erase() {
        size_t localWaveEnd = data->waveEnd;
        size_t newWaveEnd;
        if (localWaveEnd == 0) {
            newWaveEnd = data->creakersBegin - 1;
        } else {
            newWaveEnd = localWaveEnd - 1;
        }
        data->waveEnd.compare_exchange_weak(localWaveEnd, newWaveEnd);
        return data->get(newWaveEnd);
    }

    K next_insert() {
        size_t localWaveBegin = data->waveBegin;
        size_t newWaveBegin;
        if (localWaveBegin == 0) {
            newWaveBegin = data->creakersBegin - 1;
        } else {
            newWaveBegin = localWaveBegin - 1;
        }
        data->waveBegin.compare_exchange_weak(localWaveBegin, newWaveBegin);
        return data->get(newWaveBegin);
    }

    K next_range() {
        return this->next_read();
    }

    K next_prefill() {
        return data->get(data->waveBegin + prefillSize++);
    }

    K next_warming() {
        return data->get(data->creakersBegin + creakersDist->next());
    }

    ~CreakersAndWaveKeyGenerator() {
        // todo delete data
        delete creakersDist;
        delete waveDist;
    }

};


#endif //SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_H
