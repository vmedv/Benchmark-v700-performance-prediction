//
// Created by Ravil Galiev on 28.02.2023.
//

#ifndef SETBENCH_DISTRIBUTION_PARAMETERS_H
#define SETBENCH_DISTRIBUTION_PARAMETERS_H

//#include "common.h"

struct DistributionParameters {
    virtual std::string toString() = 0;

    virtual ~DistributionParameters() {};
};

#endif //SETBENCH_DISTRIBUTION_PARAMETERS_H
