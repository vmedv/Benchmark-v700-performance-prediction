//
// Created by Ravil Galiev on 11.04.2023.
//

#ifndef SETBENCH_THREAD_LOOP_BUILDER_H
#define SETBENCH_THREAD_LOOP_BUILDER_H

#include <string>
#include "thread_loop.h"
#include "thread_loop_parameters.h"
#include "thread_loop_parameters_impls.h"
#include "thread_loop_impls.h"
#include "workloads/thread_loops/thread_loop_builder.h"


struct ThreadLoopBuilder_old {
    ThreadLoopType threadLoopType;
    ThreadLoopParameters *threadLoopParameters;

    ThreadLoopBuilder_old()
            : threadLoopType(ThreadLoopType::DEFAULT),
              threadLoopParameters(new DefaultThreadLoopParameters) {}

    ThreadLoopBuilder_old(ThreadLoopParameters *threadLoopParameters)
            : threadLoopType(threadLoopParameters->threadLoopType), threadLoopParameters(threadLoopParameters) {}


    bool parseArg(ParseArgument * args) {
        return threadLoopParameters->parseArg(args);
    }

    void build() {
        threadLoopParameters->build();
    }

    std::string toString() {
        return threadLoopParameters->toString();
    }

    template<class GlobalsT>
    ThreadLoop<GlobalsT> * getThreadLoop(GlobalsT *g, int _tid) {
        switch (threadLoopType) {
            case ThreadLoopType::DEFAULT: {
                return new DefaultThreadLoop<GlobalsT>(
                        g, _tid,
                        (DefaultThreadLoopParameters *) threadLoopParameters
                );
            }
            case ThreadLoopType::TEMPORARY_OPERATION: {
                return new TemporaryOperationThreadLoop<GlobalsT>(
                        g, _tid,
                        (TemporaryOperationThreadLoopParameters *) threadLoopParameters
                );
            }
        }
    }

};

#endif //SETBENCH_THREAD_LOOP_BUILDER_H
