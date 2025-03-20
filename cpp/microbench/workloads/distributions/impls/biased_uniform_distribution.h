#pragma once

#include <cassert>
#include <cstddef>
#include "random_xoshiro256p.h"
#include "workloads/distributions/distribution.h"

class BiasedUniformDistribution : public MutableDistribution {
private:
    Random64& rng_;
    size_t range_;
    size_t bias_;

public:
    explicit BiasedUniformDistribution(Random64& rng, const size_t range = 0, size_t bias = 0)
        : rng_(rng),
          range_(range),
          bias_(bias) {
    }

    void setRange(size_t max_key) override {
        range_ = max_key;
    }

    size_t next() override {
        size_t result = rng_.next(range_) + bias_;
        return result;
    }

    ~BiasedUniformDistribution() override = default;
};

