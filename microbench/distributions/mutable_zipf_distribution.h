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

class MutableZipfDistribution : public MutableDistribution{
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

    size_t next() {
        double z; // Uniform random number (0 < z < 1)
        do {
            z = (rng->next() / (double) rng->max_value);
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

    size_t next(size_t _maxKey) {
        setMaxKey(_maxKey);
        return next();
    }
};


#endif //SETBENCH_MUTABLE_ZIPF_DISTRIBUTION_H
