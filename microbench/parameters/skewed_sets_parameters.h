//
// Created by Huawei on 26.08.2022.
//

#ifndef SETBENCH_SKEWED_SETS_PARAMETERS_H
#define SETBENCH_SKEWED_SETS_PARAMETERS_H

struct SkewedSetsParameters  {
    double READ_HOT_SIZE = 0;
    double READ_HOT_PROB = 0;
    double WRITE_HOT_SIZE = 0;
    double WRITE_HOT_PROB = 0;
    double INTERSECTION = 0;
};

#endif //SETBENCH_SKEWED_SETS_PARAMETERS_H
