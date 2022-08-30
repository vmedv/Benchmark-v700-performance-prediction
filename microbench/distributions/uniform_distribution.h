//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_UNIFORM_DISTRIBUTION_H
#define SETBENCH_UNIFORM_DISTRIBUTION_H

#include <cassert>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "distribution.h"

template<typename K>
class UniformDistribution : public Distribution<K> {
private:
    PAD;
    Random64 *rng;
    size_t maxKey;
    PAD;
public:
    UniformDistribution(Random64 *_rng, const size_t _maxKey) : rng(_rng), maxKey(_maxKey) {}

    K next() {
        auto result = 1 + rng->next(maxKey);
        assert((result >= 1) && (result <= maxKey));
        // GSTATS_ADD_IX(tid, key_gen_histogram, 1, result);
        return result;
    }
};

#endif //SETBENCH_UNIFORM_DISTRIBUTION_H
