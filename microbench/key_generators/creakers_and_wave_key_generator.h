//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_H
#define SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_H

#include <algorithm>
#include <atomic>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "key_generator.h"
#include "distributions/distribution.h"
#include "distributions/skewed_sets_distribution.h"
#include "parameters/creakers_and_wave_parameters.h"

class CreakersAndWaveKeyGeneratorData : public KeyGeneratorData {
public:
    PAD;
    size_t maxKey;

    size_t creakersLength;
    size_t defaultWaveLength;
    size_t creakersBegin;
    size_t prefillSize;

    std::atomic<size_t> waveBegin;
    std::atomic<size_t> waveEnd;
    CreakersAndWaveParameters *CWParm;

    int *data;
    PAD;

    CreakersAndWaveKeyGeneratorData(const size_t _maxKey, CreakersAndWaveParameters *_CWParm) : CWParm(_CWParm) {
        maxKey = _maxKey;
        data = new int[maxKey];
        for (int i = 0; i < maxKey; i++) {
            data[i] = i + 1;
        }

        std::random_shuffle(data, data + maxKey - 1);

        /*
         * first version where
         * data = | creakers | wave ... |
            creakersLength = (size_t) (maxKey * CWParm->CREAKERS_SIZE);
            defaultWaveLength = (size_t) (maxKey * CWParm->WAVE_SIZE);
            waveEnd = maxKey;
            waveBegin = maxKey - defaultWaveLength;
            nonCreakerLength = maxKey - creakersLength;
        */

        /*
         * data = | ... wave | creakers |
         */
        creakersLength = (size_t) (maxKey * CWParm->CREAKERS_SIZE);
        creakersBegin = maxKey - creakersLength;
        defaultWaveLength = (size_t) (maxKey * CWParm->WAVE_SIZE);
        waveEnd = creakersBegin;
        waveBegin = waveEnd - defaultWaveLength;
        prefillSize = creakersLength + defaultWaveLength;
    }

    ~CreakersAndWaveKeyGeneratorData() {
        delete[] data;
    }
};

template<typename K>
class CreakersAndWaveKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    CreakersAndWaveKeyGeneratorData *keygenData;
    Random64 *rng;
    Distribution *creakersDist;
    MutableDistribution *waveDist;
//    Distribution<K> *CWDist;
//    size_t waveBegin;
//    size_t waveLength;
    size_t prefillSize;
    PAD;

    /**
     *
     * @return true - creakers, true - wave
     */
    bool next_coin() {
        double z; // Uniform random number (0 < z < 1)

        do {
            z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
        } while ((z == 0) || (z == 1));

        return z < keygenData->CWParm->CREAKERS_PROB;
    }

public:
    CreakersAndWaveKeyGenerator(CreakersAndWaveKeyGeneratorData *_keygenData, Random64 *_rng,
                                Distribution *_creakersDist, MutableDistribution *_waveDist)
            : keygenData(_keygenData), rng(_rng), creakersDist(_creakersDist), waveDist(_waveDist) {
//        CWDist = new SkewedSetsDistribution<K>(_creakersDist, _waveDist, rng,
//                                               keygenData->CWParm->CREAKERS_PROB, keygenData->creakersLength);
//        waveBegin = keygenData->waveBegin;
//        waveLength = keygenData->defaultWaveLength;
    }

    K next_read() {
        K value;
        if (next_coin()) {
            value = keygenData->data[keygenData->creakersBegin + creakersDist->next()];
        } else {
            /**
             * In waveDist the first indexes have a higher probability
             */
            size_t localWaveBegin = keygenData->waveBegin;
            size_t localWaveEnd = keygenData->waveEnd;
            size_t localWaveLength =
                    (localWaveEnd - localWaveBegin + keygenData->creakersBegin) % keygenData->creakersBegin;
            size_t index = (localWaveBegin + waveDist->next(localWaveLength)) % keygenData->creakersBegin;
            value = keygenData->data[index];
        }
        return value;
    }

    K next_write() {
        return NULL;
    }

    K next_erase() {
        size_t localWaveEnd = keygenData->waveEnd;
        size_t newWaveEnd;
        if (localWaveEnd == 0) {
            newWaveEnd = keygenData->creakersBegin;
        } else {
            newWaveEnd = localWaveEnd - 1;
        }
        keygenData->waveEnd.compare_exchange_weak(localWaveEnd, newWaveEnd);
        return newWaveEnd;
    }

    K next_insert() {
        size_t localWaveBegin = keygenData->waveBegin;
        size_t newWaveBegin;
        if (localWaveBegin == 0) {
            newWaveBegin = keygenData->creakersBegin;
        } else {
            newWaveBegin = localWaveBegin - 1;
        }
        keygenData->waveBegin.compare_exchange_weak(localWaveBegin, newWaveBegin);
        return newWaveBegin;
    }

    K next_prefill() {
        return keygenData->data[keygenData->waveBegin + prefillSize++];
    }

    K next_warming() {
        return keygenData->data[keygenData->creakersBegin + creakersDist->next()];
    }
};


#endif //SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_H
