//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_MUTABLE_ZIPF_DISTRIBUTION_H
#define SETBENCH_MUTABLE_ZIPF_DISTRIBUTION_H

#include <algorithm>
#include <cassert>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "distribution.h"

template<typename K>
class MutableZipfDistribution : public Distribution<K>{
private:
    PAD;
    Random64 *rng;
    size_t maxKey;
    double area;
    double alpha;
    PAD;
public:
    MutableZipfDistribution(Random64 *_rng, double _alpha = 1.0) : rng(_rng), maxKey(0), area(0), alpha(_alpha) {}

    void setMaxKey(size_t _maxKey) {
        maxKey = _maxKey;
        if (alpha == 1.0) {
            area = log(maxKey);
        }
        else {
            area = pow((double) maxKey, 1.0 - alpha)/(1.0 - alpha);
        }
    }

    K next() {
        double z; // Uniform random number (0 < z < 1)
        do {
            z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
        } while ((z == 0) || (z == 1));

        size_t zipf_value = 0;
        double s = area * z;
        if (alpha == 1.0) {
            zipf_value = (size_t) exp(s);
        }
        else {
            zipf_value = (size_t) pow(s*(1-alpha), 1/(1-alpha));
        }
        return zipf_value;
    }
};


#endif //SETBENCH_MUTABLE_ZIPF_DISTRIBUTION_H
