//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_KEY_GENERATOR_H
#define SETBENCH_KEY_GENERATOR_H

enum class KeyGeneratorType {
    SIMPLE_KEYGEN, SKEWED_SETS, TEMPORARY_SKEWED, CREAKERS_AND_WAVE
};

char *keyGeneratorTypeToString(KeyGeneratorType keyGeneratorType) {
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
class KeyGenerator {
public:
    KeyGenerator() = default;

    virtual K next_read() = 0;

    virtual K next_erase() = 0;

    virtual K next_insert() = 0;

    virtual K next_prefill() = 0;
};

template<typename K>
class KeyGeneratorData {
public:
    virtual K get(size_t index) = 0;

};

template<typename K>
class KeyGeneratorBuilder {
public:
    KeyGeneratorBuilder() = default;

    KeyGeneratorType keyGeneratorType = KeyGeneratorType::SIMPLE_KEYGEN;

    virtual KeyGenerator<K> **generateKeyGenerators(size_t maxkeyToGenerate, Random64 * rngs) = 0;

};

#endif //SETBENCH_KEY_GENERATOR_H
