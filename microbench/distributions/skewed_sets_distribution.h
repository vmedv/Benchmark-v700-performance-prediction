//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_SKEWED_SETS_DISTRIBUTION_H
#define SETBENCH_SKEWED_SETS_DISTRIBUTION_H

#include <algorithm>
#include <cassert>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "distribution.h"

template<typename K>
class SkewedSetsDistribution : public Distribution<K> {
private:
    PAD;
    Random64 *rng;
    Distribution<K> *hotDistribution;
    Distribution<K> *coldDistribution;
    double hotProb;
    size_t hotSetLength;
    PAD;
public:
    SkewedSetsDistribution(Distribution<K> *_hotDistribution, Distribution<K> *_coldDistribution,
                           Random64 *_rng, const double _hotProb, const size_t _hotSetLength)
            : hotDistribution(_hotDistribution), coldDistribution(_coldDistribution),
              rng(_rng), hotProb(_hotProb), hotSetLength(_hotSetLength) {}

    K next() {
        K value;
        double z; // Uniform random number (0 < z < 1)
        // Pull a uniform random number (0 < z < 1)
        do {
            z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
        } while ((z == 0) || (z == 1));
        if (z < hotProb) {
            value = hotDistribution->next();
        } else {
            value = hotSetLength + coldDistribution->next();
        }
        return value;
    }
};


#endif //SETBENCH_SKEWED_SETS_DISTRIBUTION_H
