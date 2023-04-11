//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_DEFAULT_THREAD_LOOP_H
#define SETBENCH_DEFAULT_THREAD_LOOP_H

#include "thread_loops/thread_loop.h"
#include "thread_loops/parameters/default_thread_loop_parameters.h"

template<class GlobalsT>
class DefaultThreadLoop : public ThreadLoop<GlobalsT> {
private:
    int __tid;
    double *cdf;
    GlobalsT *g;

public:
    DefaultThreadLoop(GlobalsT *_g, int _tid, DefaultThreadLoopParameters *threadLoopParameters)
    : g(_g), __tid(_tid) {
        cdf = new double[3];
        cdf[0] = threadLoopParameters->INS_FRAC;
        cdf[1] = cdf[0] + threadLoopParameters->DEL_FRAC;
        cdf[2] = cdf[1] + threadLoopParameters->RQ;
    }

    virtual void run() {
        THREAD_MEASURED_PRE;
        while (!g->done) {
            ++cnt;
            VERBOSE if (cnt && ((cnt % 1000000) == 0)) COUTATOMICTID("op# " << cnt << std::endl);
            test_type key;

//            double op = g->rngs[tid].next(100000000) / 1000000.;
            double op = g->rngs[__tid].next() / (double) g->rngs[__tid].max_value;
            if (op < cdf[0]) { // insert
                key = g->keygens[__tid]->next_insert();

                TRACE COUTATOMICTID("### calling INSERT " << key << std::endl);
                if (g->dsAdapter->INSERT_FUNC(__tid, key, KEY_TO_VALUE(key)) == g->dsAdapter->getNoValue()) {
                    TRACE COUTATOMICTID("### completed INSERT modification for " << key << std::endl);
                    GSTATS_ADD(__tid, key_checksum, key);
                    // GSTATS_ADD(tid, size_checksum, 1);
                } else {
                    TRACE COUTATOMICTID("### completed READ-ONLY" << std::endl);
                }
                GSTATS_ADD(__tid, num_inserts, 1);
            } else if (op < cdf[1]) { // erase
                key = g->keygens[__tid]->next_erase();
                TRACE COUTATOMICTID("### calling ERASE " << key << std::endl);
                if (g->dsAdapter->erase(__tid, key) != g->dsAdapter->getNoValue()) {
                    TRACE COUTATOMICTID("### completed ERASE modification for " << key << std::endl);
                    GSTATS_ADD(__tid, key_checksum, -key);
                    // GSTATS_ADD(tid, size_checksum, -1);
                } else {
                    TRACE COUTATOMICTID("### completed READ-ONLY" << std::endl);
                }
                GSTATS_ADD(__tid, num_deletes, 1);
            } else if (op < cdf[2]) { // range query
                test_type leftKey = g->keygens[__tid]->next_range();
                test_type rightKey = g->keygens[__tid]->next_range();
                if (leftKey > rightKey) {
                    std::swap(leftKey, rightKey);
                }
                ++rq_cnt;
                size_t rqcnt;
                if ((rqcnt = g->dsAdapter->rangeQuery(__tid, leftKey, rightKey, rqResultKeys,
                                                      (VALUE_TYPE*) rqResultValues))) {
                    garbage += rqResultKeys[0] +
                               rqResultKeys[rqcnt - 1]; // prevent rqResultValues and count from being optimized out
                }
                GSTATS_ADD(__tid, num_rq, 1);
            } else { // read
                key = g->keygens[__tid]->next_read();
                if (g->dsAdapter->contains(__tid, key)) {
                    garbage += key; // prevent optimizing out
                }
                GSTATS_ADD(__tid, num_searches, 1);
            }
            // GSTATS_ADD(tid, num_operations, 1);
        }
        THREAD_MEASURED_POST;
    }

};

#endif //SETBENCH_DEFAULT_THREAD_LOOP_H
