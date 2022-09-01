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

class SkewedSetsDistribution : public Distribution {
private:
    PAD;
    Random64 *rng;
    Distribution *hotDistribution;
    Distribution *coldDistribution;
    double hotProb;
    size_t hotSetLength;
    PAD;
public:
    SkewedSetsDistribution(Distribution *_hotDistribution, Distribution *_coldDistribution,
                           Random64 *_rng, const double _hotProb, const size_t _hotSetLength)
            : hotDistribution(_hotDistribution), coldDistribution(_coldDistribution),
              rng(_rng), hotProb(_hotProb), hotSetLength(_hotSetLength) {}

    size_t next() {
        size_t value;
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
