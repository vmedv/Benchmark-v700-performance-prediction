//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_DATA_H
#define SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_DATA_H

#include <algorithm>
#include <atomic>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "key_generators/key_generator.h"
#include "distributions/distribution.h"
#include "distributions/skewed_sets_distribution.h"
#include "parameters/creakers_and_wave_parameters.h"

template<typename K>
class CreakersAndWaveKeyGeneratorData : public KeyGeneratorData<K> {
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

    size_t *data;
    PAD;

    CreakersAndWaveKeyGeneratorData(const size_t _maxKey, CreakersAndWaveParameters *_CWParm) : CWParm(_CWParm) {
        maxKey = _maxKey;
        data = new size_t[maxKey];
        for (size_t i = 0; i < maxKey; i++) {
            data[i] = i;
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

    K get(size_t index) {
        return data[index];
    }

    ~CreakersAndWaveKeyGeneratorData() {
        delete[] data;
    }
};


#endif //SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_DATA_H
