//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_THREAD_LOOP_BUILDER_H
#define SETBENCH_THREAD_LOOP_BUILDER_H

#include "thread_loop.h"

struct ThreadLoopBuilder {

    virtual ThreadLoopBuilder init(int range);

    template<class GlobalsT>
    virtual ThreadLoop<GlobalsT> build();
};

#endif //SETBENCH_THREAD_LOOP_BUILDER_H
