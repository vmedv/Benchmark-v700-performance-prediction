//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_BENCH_PARAMETERS_H
#define SETBENCH_BENCH_PARAMETERS_H

#include "parameters.h"

struct BenchParameters {
    int range;

    Parameters * test;
    Parameters * prefill;
    Parameters * warmUp;

};

#endif //SETBENCH_BENCH_PARAMETERS_H
