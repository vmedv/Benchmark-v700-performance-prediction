//
// Created by Ravil Galiev on 28.02.2023.
//

#ifndef SETBENCH_SKEWED_SET_PARAMETERS_H
#define SETBENCH_SKEWED_SET_PARAMETERS_H

#include "common.h"

struct SkewedSetParameters : public DistributionParameters {
    double hotSize;
    double hotProb;

    DistributionBuilder * hotDistBuilder;
    DistributionBuilder * coldDistBuilder;

    SkewedSetParameters(double _hotSize, double _hotProb) : hotSize(_hotSize), hotProb(_hotProb) {}

    int getHotLength(int range) {
        return (int) range * hotSize;
    }

    int getColdLength(int range) {
        return range - getHotLength(range);
    }

};

#endif //SETBENCH_SKEWED_SET_PARAMETERS_H
