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

class ZipfDistributionData {
public:
    PAD;
    size_t maxKey;
    double c = 0; // Normalization constant
    double *sum_probs; // Pre-calculated sum of probabilities
    PAD;

    ZipfDistributionData(const size_t _maxKey, const double _alpha) {
        maxKey = _maxKey;
        // Compute normalization constant c for implied key range: [1, maxKey]
        for (int i = 1; i <= _maxKey; i++) {
            c += ((double) 1) / pow((double) i, _alpha);
        }
        double *probs = new double[_maxKey + 1];
        for (int i = 1; i <= _maxKey; i++) {
            probs[i] = (((double) 1) / pow((double) i, _alpha)) / c;
        }
        // Random should be seeded already (in main)
        std::random_shuffle(probs + 1, probs + maxKey);
        sum_probs = new double[_maxKey + 1];
        sum_probs[0] = 0;
        for (int i = 1; i <= _maxKey; i++) {
            sum_probs[i] = sum_probs[i - 1] + probs[i];
        }

        delete[] probs;
    }

    ~ZipfDistributionData() {
        delete[] sum_probs;
    }
};

template<typename K>
class ZipfDistribution : public Distribution<K> {
private:
    PAD;
    ZipfDistributionData *data;
    Random64 *rng;
    PAD;
public:
    ZipfDistribution(ZipfDistributionData *_data, Random64 *_rng)
            : data(_data), rng(_rng) {}

    K next() {
        double z; // Uniform random number (0 < z < 1)
        int zipf_value = 0; // Computed exponential value to be returned
        // Pull a uniform random number (0 < z < 1)
        do {
            z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
        } while ((z == 0) || (z == 1));
        zipf_value = std::upper_bound(data->sum_probs + 1, data->sum_probs + data->maxKey + 1, z) - data->sum_probs;
        // Assert that zipf_value is between 1 and N
        assert((zipf_value >= 1) && (zipf_value <= data->maxKey));
        // GSTATS_ADD_IX(tid, key_gen_histogram, 1, zipf_value);
        return (zipf_value);
    }
};

#endif //SETBENCH_ZIPF_DISTRIBUTION_H
