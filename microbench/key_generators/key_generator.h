//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_KEY_GENERATOR_H
#define SETBENCH_KEY_GENERATOR_H

#include "common.h"
#include "random_xoshiro256p.h"
//#include "parameters.h"

enum class KeyGeneratorType {
    SIMPLE_KEYGEN, SKEWED_SETS, TEMPORARY_SKEWED, CREAKERS_AND_WAVE
};

std::string keyGeneratorTypeToString(KeyGeneratorType keyGeneratorType) {
    switch (keyGeneratorType) {
        case KeyGeneratorType::SIMPLE_KEYGEN:
            return "SIMPLE";
        case KeyGeneratorType::SKEWED_SETS:
            return "SKEWED_SETS";
        case KeyGeneratorType::TEMPORARY_SKEWED:
            return "TEMPORARY_SKEWED";
        case KeyGeneratorType::CREAKERS_AND_WAVE:
            return "CREAKERS_AND_WAVE";
    }
}

template<typename K>
struct KeyGenerator {
    KeyGenerator() = default;

    virtual K next_read() = 0;

    virtual K next_erase() = 0;

    virtual K next_insert() = 0;

    virtual K next_range() = 0;

    virtual K next_prefill() = 0;

    virtual ~KeyGenerator() = default;
};

template<typename K>
class KeyGeneratorData {
private:
    K * data = nullptr;
    bool isNonShuffle = true;

public:
    KeyGeneratorData(Parameters * _parameters) 
        : KeyGeneratorData(_parameters->MAXKEY, _parameters->isNonShuffle) {
    }

    KeyGeneratorData(int maxKey, bool _isNonShuffle) : isNonShuffle(_isNonShuffle) {
        if (!isNonShuffle) {
            data = new K[maxKey];
            for (size_t i = 0; i < maxKey; i++) {
                data[i] = i + 1;
            }

            std::random_shuffle(data, data + maxKey - 1);
        }
    }

    K get(size_t index) {
        return !isNonShuffle ? data[index] : index + 1;
    }

    virtual ~KeyGeneratorData() {
        delete[] data;
    }

};

template<typename K>
struct KeyGeneratorBuilder {
    Parameters * parameters;

    KeyGeneratorBuilder(Parameters * _parameters) : parameters(_parameters) {};

    KeyGeneratorType keyGeneratorType = KeyGeneratorType::SIMPLE_KEYGEN;

    virtual KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 * rngs) = 0;

    virtual ~KeyGeneratorBuilder() {}

};


#endif //SETBENCH_KEY_GENERATOR_H
