//
// Created by Ravil Galiev on 02.09.2022.
//

#ifndef SETBENCH_SIMPLE_PARAMETERS_H
#define SETBENCH_SIMPLE_PARAMETERS_H

struct SimpleParameters {
public:
    DistributionType distributionType = DistributionType::UNIFORM;
    double zipf_parm = 1.0;
};

#endif //SETBENCH_SIMPLE_PARAMETERS_H
