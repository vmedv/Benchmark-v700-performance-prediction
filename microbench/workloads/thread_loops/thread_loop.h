//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_THREAD_LOOP_H
#define SETBENCH_THREAD_LOOP_H

template<class GlobalsT>
class ThreadLoop{
    int threadId;
    GlobalsT *g;

    void run() {

    }

    virtual void step();
};

#endif //SETBENCH_THREAD_LOOP_H
