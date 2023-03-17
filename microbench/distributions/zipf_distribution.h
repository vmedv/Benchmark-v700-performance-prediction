//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_ZIPF_DISTRIBUTION_H
#define SETBENCH_ZIPF_DISTRIBUTION_H

#include <algorithm>
#include <cassert>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "distribution.h"

class ZipfDistribution : public MutableDistribution {
private:
    PAD;
    Random64 *rng;
    size_t maxKey;
    double area;
    double alpha;
    PAD;
public:

    ZipfDistribution(Random64 *_rng, double _alpha = 1.0, size_t _maxkey = 0)
            : rng(_rng), maxKey(_maxkey), area(0), alpha(_alpha) {}

    void setMaxKey(size_t _maxKey) override {
        maxKey = _maxKey;
        if (alpha == 1.0) {
            area = log(maxKey);
        } else {
            area = (pow((double) maxKey, 1.0 - alpha) - 1.0) / (1.0 - alpha);
        }
    }

    size_t next() override {
        double z; // Uniform random number (0 < z < 1)
        do {
            z = (rng->next() / (double) rng->max_value);
        } while ((z == 0) || (z == 1));

        size_t zipf_value = 0;
        double s = area * z;
        if (alpha == 1.0) {
            zipf_value = (size_t) exp(s);
        } else {
            zipf_value = (size_t) pow(s * (1.0 - alpha) + 1.0, 1.0 / (1.0 - alpha));
        }
        return zipf_value;
    }

    size_t next(size_t _maxKey) override {
        setMaxKey(_maxKey);
        return next();
    }

    ~ZipfDistribution() {}
};


#endif //SETBENCH_ZIPF_DISTRIBUTION_H
