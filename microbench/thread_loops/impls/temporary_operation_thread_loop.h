//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_TEMPORARY_OPERATION_THREAD_LOOP_H
#define SETBENCH_TEMPORARY_OPERATION_THREAD_LOOP_H

#include "thread_loops/thread_loop.h"
#include "thread_loops/parameters/temporary_operation_thread_loop_parameters.h"

template<class GlobalsT>
class TemporaryOperationThreadLoop : public ThreadLoop<GlobalsT> {
private:
    int __tid;
    double **cdf;
    GlobalsT *g;
    TemporaryOperationThreadLoopParameters *loopParameters;

    size_t time;
    size_t pointer;

    void update_pointer() {
        if (time >= loopParameters->opTimes[pointer]) {
            time = 0;
            ++pointer;
            if (pointer >= loopParameters->tempOperCount) {
                pointer = 0;
            }
        }
        ++time;
    }

public:
    TemporaryOperationThreadLoop(GlobalsT *_g, int _tid, TemporaryOperationThreadLoopParameters *threadLoopParameters)
            : g(_g), __tid(_tid), loopParameters(threadLoopParameters) {
        cdf = new double *[threadLoopParameters->tempOperCount];
        time = 0;
        pointer = 0;
        for (int i = 0; i < threadLoopParameters->tempOperCount; ++i) {
            cdf[i] = new double[3];
            cdf[i][0] = threadLoopParameters->INS_FRACs[i];
            cdf[i][1] = cdf[i][0] + threadLoopParameters->DEL_FRACs[i];
            cdf[i][2] = cdf[i][1] + threadLoopParameters->RQs[i];
        }
    }

    virtual void run() {
        THREAD_MEASURED_PRE;
        while (!g->done) {
            update_pointer();
            ++cnt;
            VERBOSE if (cnt && ((cnt % 1000000) == 0)) COUTATOMICTID("op# " << cnt << std::endl);
            test_type key;

//            double op = g->rngs[tid].next(100000000) / 1000000.;
            double op = g->rngs[tid].nextDouble();
            if (op < cdf[pointer][0]) { // insert
                key = g->keygens[tid]->next_insert();

                TRACE COUTATOMICTID("### calling INSERT " << key << std::endl);
                if (g->dsAdapter->INSERT_FUNC(tid, key, KEY_TO_VALUE(key)) == g->dsAdapter->getNoValue()) {
                    TRACE COUTATOMICTID("### completed INSERT modification for " << key << std::endl);
                    GSTATS_ADD(tid, key_checksum, key);
                    // GSTATS_ADD(tid, size_checksum, 1);
                } else {
                    TRACE COUTATOMICTID("### completed READ-ONLY" << std::endl);
                }
                GSTATS_ADD(tid, num_inserts, 1);
            } else if (op < cdf[pointer][1]) { // erase
                key = g->keygens[tid]->next_erase();
                TRACE COUTATOMICTID("### calling ERASE " << key << std::endl);
                if (g->dsAdapter->erase(tid, key) != g->dsAdapter->getNoValue()) {
                    TRACE COUTATOMICTID("### completed ERASE modification for " << key << std::endl);
                    GSTATS_ADD(tid, key_checksum, -key);
                    // GSTATS_ADD(tid, size_checksum, -1);
                } else {
                    TRACE COUTATOMICTID("### completed READ-ONLY" << std::endl);
                }
                GSTATS_ADD(tid, num_deletes, 1);
            } else if (op < cdf[pointer][2]) { // range query
                test_type leftKey = g->keygens[tid]->next_range();
                test_type rightKey = g->keygens[tid]->next_range();
                if (leftKey > rightKey) {
                    std::swap(leftKey, rightKey);
                }
                ++rq_cnt;
                size_t rqcnt;
                if ((rqcnt = g->dsAdapter->rangeQuery(tid, leftKey, rightKey, rqResultKeys,
                                                      (VALUE_TYPE*) rqResultValues))) {
                    garbage += rqResultKeys[0] +
                               rqResultKeys[rqcnt - 1]; // prevent rqResultValues and count from being optimized out
                }
                GSTATS_ADD(tid, num_rq, 1);
            } else { // read
                key = g->keygens[tid]->next_read();
                if (g->dsAdapter->contains(tid, key)) {
                    garbage += key; // prevent optimizing out
                }
                GSTATS_ADD(tid, num_searches, 1);
            }
            // GSTATS_ADD(tid, num_operations, 1);
        }
        THREAD_MEASURED_POST;
    }

};

#endif //SETBENCH_TEMPORARY_OPERATION_THREAD_LOOP_H
