#pragma once

#include <cassert>
#include "random_xoshiro256p.h"
#include "workloads/distributions/distribution_builder.h"
#include "workloads/distributions/impls/biased_uniform_distribution.h"
#include "workloads/distributions/impls/uniform_distribution.h"
#include "globals_extern.h"

struct BiasedUniformDistributionBuilder : public MutableDistributionBuilder {
		BiasedUniformDistributionBuilder()  = default;
    BiasedUniformDistribution * build(Random64 &rng, size_t range) override {
        return new BiasedUniformDistribution(rng, range);
    }

    BiasedUniformDistribution * build(Random64 &rng) override {
        return new BiasedUniformDistribution(rng);
    }

    void toJson(nlohmann::json &j) const override {
        j["ClassName"] = "BiasedUniformDistributionBuilder";
    }

    void fromJson(const nlohmann::json &j) override {

    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "BiasedUniform", indents);
    }

    ~BiasedUniformDistributionBuilder() override = default;
};

