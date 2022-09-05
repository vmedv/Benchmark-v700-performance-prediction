//
// Created by Ravil Galiev on 02.09.2022.
//

#ifndef SETBENCH_SIMPLE_PARAMETERS_H
#define SETBENCH_SIMPLE_PARAMETERS_H

struct SimpleParameters {
public:
    DistributionType distributionType = UNIFORM;
    double zipf_parm;
};

#endif //SETBENCH_SIMPLE_PARAMETERS_H
