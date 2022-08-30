//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_SIMPLE_KEY_GENERATOR_H
#define SETBENCH_SIMPLE_KEY_GENERATOR_H

#include "plaf.h"
#include "key_generator.h"
#include "distributions/distribution.h"

template<typename K>
class SimpleKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    Distribution<K> *distribution;
    PAD;
public:
    SimpleKeyGenerator(Distribution<K> *_distribution) : distribution(_distribution) {}

    K next_read() {
        return distribution->next();
    }

    K next_erase() {
        return distribution->next();
    }

    K next_insert() {
        return distribution->next();
    }

    K next_prefill() {
        return distribution->next();
    }
};

#endif //SETBENCH_SIMPLE_KEY_GENERATOR_H
