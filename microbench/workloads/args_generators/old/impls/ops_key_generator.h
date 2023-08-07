//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_OPS_KEY_GENERATOR_H
#define SETBENCH_OPS_KEY_GENERATOR_H

#include "plaf.h"
#include "common.h"

template<typename K>
class OpsKeyGenerator : public KeyGenerator<K> {
private:
    PAD;
    Distribution *read_dist;
    Distribution *insert_dist;
    Distribution *remove_dist;
    KeyGeneratorData<K> *data;
    Distribution *prefillDistribution;
    bool prefill_sequential;
    size_t prefillPointer;
    PAD;

public:
    OpsKeyGenerator(KeyGeneratorData<K> *_data,
                    Distribution *_read_dist,
                    Distribution *_insert_dist,
                    Distribution *_remove_dist,
                    Distribution *_prefillDistribution,
                    bool prefill_sequential = false)
            : data(_data), read_dist(_read_dist), remove_dist(_remove_dist), insert_dist(_insert_dist),
              prefillDistribution(_prefillDistribution),
              prefill_sequential(prefill_sequential), prefillPointer(0) {
    }


    K next_read() {
        K index = read_dist->next();
        return data->get(index);
    }

    K next_insert() {
        K index = insert_dist->next();
        return data->get(index);
    }

    K next_erase() {
        K index = remove_dist->next();
        return data->get(index);
    }

    K next_range() {
        return next_read();
    }

    K next_prefill() {
        return data->get(prefill_sequential ? prefillPointer++ : prefillDistribution->next());
    }

    ~OpsKeyGenerator() {
        delete read_dist;
        delete insert_dist;
        delete remove_dist;
    }
};

#endif //SETBENCH_OPS_KEY_GENERATOR_H
