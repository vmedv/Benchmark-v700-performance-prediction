//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_THREAD_LOOP_H
#define SETBENCH_THREAD_LOOP_H

enum class ThreadLoopType {
    DEFAULT, TEMPORARY_OPERATION
};

template<class GlobalsT>
class ThreadLoop {
protected:
    int __tid;
    int RQ_RANGE;
    GlobalsT *g;

public:
    ThreadLoop(GlobalsT *g, int _tid, int rqRange) : g(g), __tid(_tid), RQ_RANGE(rqRange) {}

    ThreadLoop() = default;

    virtual void run() = 0;

    virtual ~ThreadLoop() = default;
};

#define THREAD_MEASURED_PRE \
    tid = this->__tid; \
    binding_bindThread(tid); \
    test_type garbage = 0; \
    test_type * rqResultKeys = new test_type[this->RQ_RANGE+MAX_KEYS_PER_NODE]; \
    VALUE_TYPE * rqResultValues = new VALUE_TYPE[this->RQ_RANGE+MAX_KEYS_PER_NODE]; \
    __RLU_INIT_THREAD; \
    __RCU_INIT_THREAD; \
    this->g->dsAdapter->initThread(tid); \
    papi_create_eventset(tid); \
    __sync_fetch_and_add(&this->g->running, 1); \
    __sync_synchronize(); \
    while (!this->g->start) { SOFTWARE_BARRIER; TRACE COUTATOMICTID("waiting to start"<<std::endl); } \
    GSTATS_SET(tid, time_thread_start, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - this->g->startTime).count()); \
    papi_start_counters(tid); \
    int cnt = 0; \
    int rq_cnt = 0; \
    DURATION_START(tid);

#define THREAD_MEASURED_POST \
    __sync_fetch_and_add(&this->g->running, -1); \
    DURATION_END(tid, duration_all_ops); \
    GSTATS_SET(tid, time_thread_terminate, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - this->g->startTime).count()); \
    SOFTWARE_BARRIER; \
    papi_stop_counters(tid); \
    SOFTWARE_BARRIER; \
    while (this->g->running) { SOFTWARE_BARRIER; } \
    this->g->dsAdapter->deinitThread(tid); \
    __RCU_DEINIT_THREAD; \
    __RLU_DEINIT_THREAD; \
    delete[] rqResultKeys; \
    delete[] rqResultValues; \
    this->g->garbage += garbage;


#endif //SETBENCH_THREAD_LOOP_H
