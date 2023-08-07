//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_THREAD_LOOP_H
#define SETBENCH_THREAD_LOOP_H

#include "workloads/stop_condition/stop_condition.h"
//#include "globals.h"
#include "globals_t.h"
//#include "globals_t_impl.h"
//#include "globals_extern.h"

#define VALUE_TYPE void *

typedef long long K;

//template<class GlobalsT>
class ThreadLoop {
    K garbage = 0;
    K * rqResultKeys;
    VALUE_TYPE * rqResultValues;
    int rq_cnt;
    size_t RQ_RANGE;
public:
//    int tid;
    size_t threadId;
    globals_t *g;
    StopCondition *stopCondition;

    ThreadLoop(globals_t *_g, size_t _threadId, StopCondition *_stopCondition, size_t _RQ_RANGE)
            : g(_g), threadId(_threadId), stopCondition(_stopCondition), RQ_RANGE(_RQ_RANGE) {}

    template<typename K>
    void executeInsert(K & key);

    template<typename K>
    void executeRemove(const K & key);

    template<typename K>
    void executeGet(const K & key);

    template<typename K>
    bool executeContains(const K & key);

    /**
     * the result is in the arrays rqResultKeys and rqResultValues
     */
    template<typename K>
    void executeRangeQuery(const K & leftKey, const K & rightKey);

    virtual void run();

    virtual void step() = 0;
};

#endif //SETBENCH_THREAD_LOOP_H













