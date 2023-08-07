//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_SKEWED_UNIFORM_DISTRIBUTION_BUILDER_H
#define SETBENCH_SKEWED_UNIFORM_DISTRIBUTION_BUILDER_H

#include <algorithm>
#include <cassert>
#include "random_xoshiro256p.h"
#include "plaf.h"
#include "workloads/distributions/distribution.h"
#include "workloads/distributions/distribution_builder.h"
#include "uniform_distribution_builder.h"
#include "workloads/distributions/impls/skewed_uniform_distribution.h"
//#include "workloads/distributions/distribution_json_convector.h"

DistributionBuilder *getDistributionFromJson(const nlohmann::json &j);

struct SkewedUniformDistributionBuilder : public DistributionBuilder {
    PAD;
    double hotSize = 0;
    double hotProb = 0;

    DistributionBuilder *hotDistBuilder = new UniformDistributionBuilder();
    DistributionBuilder *coldDistBuilder = new UniformDistributionBuilder();
    PAD;

    SkewedUniformDistributionBuilder *setHotSize(double _hotSize) {
        hotSize = _hotSize;
        return this;
    }

    SkewedUniformDistributionBuilder *setHotProb(double _hotProb) {
        hotProb = _hotProb;
        return this;
    }

    SkewedUniformDistributionBuilder *setHotDistBuilder(DistributionBuilder *_hotDistBuilder) {
        hotDistBuilder = _hotDistBuilder;
        return this;
    }

    SkewedUniformDistributionBuilder *setColdDistBuilder(DistributionBuilder *_coldDistBuilder) {
        coldDistBuilder = _coldDistBuilder;
        return this;
    }

    size_t getHotLength(size_t range) const {
        return (size_t) (range * hotSize);
    }

    size_t getColdLength(size_t range) const {
        return range - getHotLength(range);
    }

    SkewedUniformDistribution *build(Random64 &rng, size_t range) override {
        return new SkewedUniformDistribution(
                rng,
                hotDistBuilder->build(rng, getHotLength(range)),
                coldDistBuilder->build(rng, getColdLength(range)),
                hotProb, getHotLength(range)
        );
    }

    void toJson(nlohmann::json &j) const override {
        j["distributionType"] = DistributionType::SKEWED_UNIFORM;
        j["hotSize"] = hotSize;
        j["hotProb"] = hotProb;
        j["hotDistBuilder"] = *hotDistBuilder;
        j["coldDistBuilder"] = *coldDistBuilder;
    }

    void fromJson(const nlohmann::json &j) override {
        hotSize = j["hotSize"];
        hotProb = j["hotProb"];

        hotDistBuilder = getDistributionFromJson(j["hotDistBuilder"]);
        coldDistBuilder = getDistributionFromJson(j["coldDistBuilder"]);
//        j["coldDistBuilder"] = *coldDistBuilder;
    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "Skewed Uniform", indents)
               + indented_title_with_data("HOT SIZE", hotSize, indents)
               + indented_title_with_data("HOT PROB", hotProb, indents);
    }

    ~SkewedUniformDistributionBuilder() override {
        delete hotDistBuilder;
        delete coldDistBuilder;
    }
};


#endif //SETBENCH_SKEWED_UNIFORM_DISTRIBUTION_BUILDER_H
