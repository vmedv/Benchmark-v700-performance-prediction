//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_DATA_H
#define SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_DATA_H

#include <algorithm>
#include <atomic>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"

template<typename K>
struct CreakersAndWaveKeyGeneratorData : public KeyGeneratorData<K> {
    PAD;
    size_t maxKey;

    size_t creakersLength;
    size_t defaultWaveLength;
    size_t creakersBegin;
    size_t prefillSize;

    std::atomic<size_t> waveBegin;
    std::atomic<size_t> waveEnd;
    CreakersAndWaveParameters *parameters;

    PAD;

    CreakersAndWaveKeyGeneratorData(CreakersAndWaveParameters *_parameters)
            : KeyGeneratorData<K>(_parameters), maxKey(_parameters->MAXKEY), parameters(_parameters) {
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
        creakersLength = (size_t) (maxKey * parameters->CREAKERS_SIZE);
        creakersBegin = maxKey - creakersLength;
        defaultWaveLength = (size_t) (maxKey * parameters->WAVE_SIZE);
        waveEnd = creakersBegin;
        waveBegin = waveEnd - defaultWaveLength;
        prefillSize = creakersLength + defaultWaveLength;
    }

};


#endif //SETBENCH_CREAKERS_AND_WAVE_KEY_GENERATOR_DATA_H
