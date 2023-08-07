//
// Created by Ravil Galiev on 28.02.2023.
//

#ifndef SETBENCH_ZIPF_PARAMETERS_H
#define SETBENCH_ZIPF_PARAMETERS_H

#include "common.h"

struct ZipfParameters : public DistributionParameters {
    double alpha;

    ZipfParameters(double _alpha) : alpha(_alpha) {}

    std::string toString() override {
        return "    Zipf Parm:                : " + std::to_string(alpha) + "\n";
    }
};


#endif //SETBENCH_ZIPF_PARAMETERS_H
