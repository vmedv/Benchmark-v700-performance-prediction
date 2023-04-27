//
// Created by Ravil Galiev on 28.02.2023.
//

#ifndef SETBENCH_SKEWED_SET_PARAMETERS_H
#define SETBENCH_SKEWED_SET_PARAMETERS_H

#include "common.h"

struct SkewedUniformParameters : public DistributionParameters {
    double hotSize;
    double hotProb;

    DistributionBuilder *hotDistBuilder = new DistributionBuilder();
    DistributionBuilder *coldDistBuilder = new DistributionBuilder();

    SkewedUniformParameters(double _hotSize, double _hotProb) : hotSize(_hotSize), hotProb(_hotProb) {}

    size_t getHotLength(size_t range) {
        return (size_t) (range * hotSize);
    }

    size_t getColdLength(size_t range) {
        return range - getHotLength(range);
    }

    std::string toString() override {
        return "    HOT SIZE:                 : " + std::to_string(hotSize) + "\n" +
               "    HOT PROB:                 : " + std::to_string(hotProb) + "\n";
    }

};

#endif //SETBENCH_SKEWED_SET_PARAMETERS_H
