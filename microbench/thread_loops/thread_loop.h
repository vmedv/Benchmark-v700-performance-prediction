//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_THREAD_LOOP_H
#define SETBENCH_THREAD_LOOP_H

enum class ThreadLoopType {
    DEFAULT, TEMPORARY_OPERATION
};

template<class GlobalsT>
struct ThreadLoop {
    ThreadLoop() = default;

    virtual void run() = 0;

    virtual ~ThreadLoop() = default;
};

#endif //SETBENCH_THREAD_LOOP_H
