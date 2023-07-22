//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_PARAMETERS_H
#define SETBENCH_PARAMETERS_H

#include <vector>
#include "workloads/stop_condition/stop_condition.h"
#include "workloads/thread_loops/thread_loop_builder.h"

struct Parameters {
    int numThreads = 1;

    StopCondition stopCondition;

    std::vector<ThreadLoopBuilder*> threadLoopBuilders;

    void init(int range) {
        for (ThreadLoopBuilder * threadLoopBuilder : threadLoopBuilders) {
            threadLoopBuilder->init(range);
        }
    }
};

#endif //SETBENCH_PARAMETERS_H
