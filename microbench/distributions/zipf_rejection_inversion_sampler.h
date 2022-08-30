//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_ZIPF_REJECTION_INVERSION_SAMPLER_H
#define SETBENCH_ZIPF_REJECTION_INVERSION_SAMPLER_H

#include <algorithm>
#include <cassert>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "distribution.h"

// Sampler taken from https://commons.apache.org/proper/commons-math/apidocs/src-html/org/apache/commons/math4/distribution/ZipfDistribution.html#line.44
// Paper: Rejection-Inversion to Generate Variates from Monotone Discrete Distributions.
struct ZipfRejectionInversionSamplerData {
    int *mapping;
    const int maxkey;

    ZipfRejectionInversionSamplerData(int _maxkey) : maxkey(_maxkey) {
        mapping = new int[maxkey + 1];
#pragma omp parallel for
        for (int i = 0; i < maxkey + 1; ++i) {
            mapping[i] = i;
        }
        std::random_shuffle(mapping + 1, mapping + maxkey);
    }

    ~ZipfRejectionInversionSamplerData() {
        delete[] mapping;
    }
};


class ZipfRejectionInversionSampler : public Distribution<long long> {
    const double exponent;
    const int maxkey;
    Random64 *rng;
    ZipfRejectionInversionSamplerData *const data;
    double hIntegralX1;
    double hIntegralmaxkey;
    double s;

    double hIntegral(const double x) {
        return helper2((1 - exponent) * log(x)) * log(x);
    }

    double h(const double x) {
        return exp(-exponent * log(x));
    }

    double hIntegralInverse(const double x) {
        double t = x * (1 - exponent);
        if (t < -1) {
            // Limit value to the range [-1, +inf).
            // t could be smaller than -1 in some rare cases due to numerical errors.
            t = -1;
        }
        return exp(helper1(t) * x);
    }

    double helper1(const double x) {
        // if (abs(x)>1e-8) {
        return log(x + 1) / x;
        // }
        // else {
        //     return 1.-x*((1./2.)-x*((1./3.)-x*(1./4.)));
        // }
    }

    double helper2(const double x) {
        // if (FastMath.abs(x)>1e-8) {
        return (exp(x) - 1) / x;
        // }
        // else {
        //     return 1.+x*(1./2.)*(1.+x*(1./3.)*(1.+x*(1./4.)));
        // }
    }

public:
    /** Simple constructor.
     * @param maxkey number of elements
     * @param exponent exponent parameter of the distributions
     */
    ZipfRejectionInversionSampler(ZipfRejectionInversionSamplerData *const _data, const double _exponent,
                                  Random64 *_rng) : data(_data), maxkey(_data->maxkey), exponent(_exponent), rng(_rng) {
        if (exponent <= 1) {
            std::cout << "-dist-zipf-fast only works with exponents greater than 1." << std::endl;
            exit(-1);
        }
        hIntegralX1 = hIntegral(1.5) - 1;
        hIntegralmaxkey = hIntegral(maxkey + 0.5);
        s = 2 - hIntegralInverse(hIntegral(2.5) - h(2));
    }

    /** Generate one integral number in the range [1, maxkey].
     * @param random random generator to use
     * @return generated integral number in the range [1, maxkey]
     */
    long long next() {
        while (true) {
            // Pull a uniform random number (0 < z < 1)
            const double z = (rng->next() / (double) std::numeric_limits<uint64_t>::max());
            const double u = hIntegralmaxkey + z * (hIntegralX1 - hIntegralmaxkey);
            // u is uniformly distributed in (hIntegralX1, hIntegralmaxkey]

            double x = hIntegralInverse(u);

            int k = (int) (x + 0.5);

            if (k < 1) {
                k = 1;
            } else if (k > maxkey) {
                k = maxkey;
            }

            if (k - x <= s || u >= hIntegral(k + 0.5) - h(k)) {
                return data->mapping[k];
            }
        }
    }
};



#endif //SETBENCH_ZIPF_REJECTION_INVERSION_SAMPLER_H
