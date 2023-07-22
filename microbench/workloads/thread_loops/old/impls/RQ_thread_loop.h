//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_RQ_THREAD_LOOP_H
#define SETBENCH_RQ_THREAD_LOOP_H

#include "thread_loops/thread_loop.h"

template<class GlobalsT>
class RQThreadLoop : public ThreadLoop<GlobalsT> {
public:
    RQThreadLoop(GlobalsT *_g, int _tid, int RQ_RANGE)
    : ThreadLoop<GlobalsT>(_g, _tid, RQ_RANGE) {}

    virtual void run() {
        THREAD_MEASURED_PRE;
        while (!this->g->done) {
            test_type leftKey = this->g->keygens[tid]->next_range();
            test_type rightKey = this->g->keygens[tid]->next_range();
            if (leftKey > rightKey) {
                std::swap(leftKey, rightKey);
            }

            size_t rqcnt;
            if ((rqcnt = this->g->dsAdapter->rangeQuery(tid, leftKey, rightKey, rqResultKeys,
                                                  (VALUE_TYPE*) rqResultValues))) {
                garbage += rqResultKeys[0] +
                           rqResultKeys[rqcnt - 1]; // prevent rqResultValues and count from being optimized out
            }
            TIMELINE_END(tid, "RQThreadOperation");
            GSTATS_ADD(tid, num_rq, 1);
            GSTATS_ADD(tid, num_operations, 1);
        }
        THREAD_MEASURED_POST;
    }

};

#endif //SETBENCH_RQ_THREAD_LOOP_H
