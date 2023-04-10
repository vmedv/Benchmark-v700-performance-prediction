//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_THREAD_LOOP_PARAMETERS_H
#define SETBENCH_THREAD_LOOP_PARAMETERS_H

#include "parse_argument.h"
#include "thread_loop.h"

struct ThreadLoopParameters {
    ThreadLoopType threadLoopType = ThreadLoopType::DEFAULT;

    ThreadLoopParameters() = default;

    virtual std::string toString() = 0;

    virtual void build() = 0;

    virtual ~ThreadLoopParameters() = default;

    virtual bool parseArg(ParseArgument * args) = 0;
};

#endif //SETBENCH_THREAD_LOOP_PARAMETERS_H
