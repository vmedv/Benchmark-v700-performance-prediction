//
// Created by Ravil Galiev on 10.02.2023.
//

#ifndef SETBENCH_SKEWED_SETS_KEY_GENERATOR_DATA_H
#define SETBENCH_SKEWED_SETS_KEY_GENERATOR_DATA_H

#include <algorithm>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "common.h"


template<typename K>
struct SkewedSetsKeyGeneratorData : public KeyGeneratorData<K> {
    PAD;
    size_t maxKey;
    size_t readSetLength;
    size_t writeSetLength;

    size_t writeSetBegin;
    size_t writeSetEnd;

    SkewedSetsParameters *parameters;

    PAD;

    SkewedSetsKeyGeneratorData(SkewedSetsParameters *_parameters)
            : KeyGeneratorData<K>(_parameters), maxKey(_parameters->MAXKEY), parameters(_parameters) {
        readSetLength = (size_t) (maxKey * parameters->READ_HOT_SIZE);
        writeSetLength = (size_t) (maxKey * parameters->WRITE_HOT_SIZE);
        size_t interSetLength = (size_t) (maxKey * parameters->INTERSECTION);

        writeSetBegin = readSetLength - interSetLength;
        writeSetEnd = writeSetBegin + writeSetLength;
    }

};

#endif //SETBENCH_SKEWED_SETS_KEY_GENERATOR_DATA_H
