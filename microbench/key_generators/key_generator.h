//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_KEY_GENERATOR_H
#define SETBENCH_KEY_GENERATOR_H

enum KeyGeneratorType {
    SIMPLE_KEYGEN, SKEWED_SETS, TEMPORARY_SKEWED, CREAKERS_AND_WAVE
};

template<typename K>
class KeyGenerator {
public:
    KeyGenerator() = default;

    virtual K next_read() = 0;

    virtual K next_erase() = 0;

    virtual K next_insert() = 0;

    virtual K next_prefill() = 0;
};

class KeyGeneratorData {

};

#endif //SETBENCH_KEY_GENERATOR_H
