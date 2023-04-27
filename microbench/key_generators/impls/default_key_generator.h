//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_DEFAULT_KEY_GENERATOR_H
#define SETBENCH_DEFAULT_KEY_GENERATOR_H

#include "plaf.h"
#include "common.h"

template<typename K>
class DefaultKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    Distribution *distribution;
    KeyGeneratorData<K> *data;
    Distribution *prefillDistribution;
    bool prefill_sequential;
    size_t prefillPointer;
    PAD;

    K next() {
        K index = distribution->next();
        return data->get(index);
    }

public:
    DefaultKeyGenerator(KeyGeneratorData<K> *_data,
                        Distribution *_distribution,
                        Distribution *_prefillDistribution,
                        bool prefill_sequential = false)
            : data(_data), distribution(_distribution), prefillDistribution(_prefillDistribution),
              prefill_sequential(prefill_sequential), prefillPointer(0) {
    }


    K next_read() {
        return next();
    }

    K next_erase() {
        return next();
    }

    K next_insert() {
        return next();
    }

    K next_range() {
        return next();
    }

    K next_prefill() {
        return data->get(prefill_sequential ? prefillPointer++ : prefillDistribution->next());
    }

    ~DefaultKeyGenerator() {
        delete distribution;
    }
};

#endif //SETBENCH_DEFAULT_KEY_GENERATOR_H
