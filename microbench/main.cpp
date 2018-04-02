/**
 * Setbench test harness for performing rigorous data structure microbenchmarks.
 * 
 * Copyright (C) 2018 Trevor Brown
 */

#define MICROBENCH

typedef long long test_type;

#define USE_GSTATS
#include <limits>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <atomic>
#include <chrono>
#include <cassert>

/**
 * Configure global statistics using gstats_global.h and gstats.h
 */

#define __AND ,
#define GSTATS_HANDLE_STATS(gstats_handle_stat) \
    gstats_handle_stat(LONG_LONG, node_allocated_addresses, 100, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, descriptor_allocated_addresses, 100, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, extra_type1_allocated_addresses, 100, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, extra_type2_allocated_addresses, 100, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, extra_type3_allocated_addresses, 100, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, extra_type4_allocated_addresses, 100, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, num_updates, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_searches, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_rq, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_operations, 1, { \
            gstats_output_item(PRINT_RAW, SUM, BY_THREAD) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
/*    gstats_handle_stat(LONG_LONG, num_getguard, 1, { \
            gstats_output_item(PRINT_RAW, SUM, BY_THREAD) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_tryadvance, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_root, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_parentlarge, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_nodes_visited, 1, { \
            gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_hist_readepoch, 1, { \
            gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_root_update_time, 1, { \
            gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_root_read_time, 1, { \
            gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, timer_guard_latency, 1, {}) \
    gstats_handle_stat(LONG_LONG, timer_epoch_latency, 1, {}) \
    gstats_handle_stat(LONG_LONG, num_prop_epoch_latency, 100000, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_guard_latency, 100000, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_guard_insdel_attempts, 100000, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_guard_rebalance_attempts, 100000, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_epoch_allocations, 500000, { \
            gstats_output_item(PRINT_RAW, SUM, BY_INDEX) \
    }) \
    gstats_handle_stat(LONG_LONG, num_prop_thread_exit_time, 1, { \
            gstats_output_item(PRINT_RAW, FIRST, BY_THREAD) \
    }) \
    gstats_handle_stat(LONG_LONG, thread_announced_epoch, 1, {})*/ \
    gstats_handle_stat(LONG_LONG, visited_in_bags, 1, { \
            /*gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL)*/ \
    }) \
    gstats_handle_stat(LONG_LONG, skipped_in_bags, 1, { \
            /*gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL)*/ \
    }) \
    gstats_handle_stat(LONG_LONG, latency_rqs, 1, { \
            /*gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL)*/ \
    }) \
    gstats_handle_stat(LONG_LONG, latency_updates, 1, { \
            /*gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL)*/ \
    }) \
    gstats_handle_stat(LONG_LONG, latency_searches, 1, { \
            /*gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL)*/ \
    }) \
    gstats_handle_stat(LONG_LONG, key_checksum, 1, {}) \
    gstats_handle_stat(LONG_LONG, prefill_size, 1, {}) \
    gstats_handle_stat(LONG_LONG, timer_latency, 1, {})


#include "globals.h"
#include "globals_extern.h"
#include "random.h"
#include "plaf.h"
#include "binding.h"
#include "papi_util_impl.h"
#include "rq_provider.h"

#include "adapter.h" /* data structure adapter header (selected according to the "ds/..." subdirectory in the -I include paths */

#ifndef STR
    #define STR(x) XSTR(x)
    #define XSTR(x) #x
#endif

#ifndef INSERT_FUNC
    #define INSERT_FUNC insertIfAbsent
#endif

#ifdef RQ_SNAPCOLLECTOR
    #define RQ_SNAPCOLLECTOR_OBJECT_TYPES , SnapCollector<node_t<test_type, test_type>, test_type>, SnapCollector<node_t<test_type, test_type>, test_type>::NodeWrapper, ReportItem, CompactReportItem
    #define RQ_SNAPCOLLECTOR_OBJ_SIZES <<" SnapCollector="<<(sizeof(SnapCollector<node_t<test_type, test_type>, test_type>))<<" NodeWrapper="<<(sizeof(SnapCollector<node_t<test_type, test_type>, test_type>::NodeWrapper))<<" ReportItem="<<(sizeof(ReportItem))<<" CompactReportItem="<<(sizeof(CompactReportItem))
#else
    #define RQ_SNAPCOLLECTOR_OBJECT_TYPES 
    #define RQ_SNAPCOLLECTOR_OBJ_SIZES 
#endif

#define KEY_TO_VALUE(key) &key /*((void*) (size_t) (key))*/ /* note: unsafe hack to get a pointer, but no one ever follows these pointers */
#define VALUE_TYPE void *

#ifdef USE_RCU
    #include "eer_prcu_impl.h"
    #define __RCU_INIT_THREAD urcu::registerThread(tid);
    #define __RCU_DEINIT_THREAD urcu::unregisterThread();
    #define __RCU_INIT_ALL urcu::init(TOTAL_THREADS);
    #define __RCU_DEINIT_ALL urcu::deinit(TOTAL_THREADS);
#else
    #define __RCU_INIT_THREAD 
    #define __RCU_DEINIT_THREAD 
    #define __RCU_INIT_ALL 
    #define __RCU_DEINIT_ALL 
#endif

#ifdef USE_RLU
    #include "rlu.h"
    PAD;
    __thread rlu_thread_data_t * rlu_self;
    PAD;
    rlu_thread_data_t * rlu_tdata = NULL;
    #define __RLU_INIT_THREAD rlu_self = &rlu_tdata[tid]; RLU_THREAD_INIT(rlu_self);
    #define __RLU_DEINIT_THREAD RLU_THREAD_FINISH(rlu_self);
    #define __RLU_INIT_ALL rlu_tdata = new rlu_thread_data_t[MAX_THREADS_POW2]; RLU_INIT(RLU_TYPE_FINE_GRAINED, 1);
    #define __RLU_DEINIT_ALL RLU_FINISH(); delete[] rlu_tdata;
#else
    #define __RLU_INIT_THREAD 
    #define __RLU_DEINIT_THREAD 
    #define __RLU_INIT_ALL 
    #define __RLU_DEINIT_ALL 
#endif

#define INIT_THREAD(tid) \
    __RLU_INIT_THREAD; \
    __RCU_INIT_THREAD; \
    g.ds->initThread(tid);
#define DEINIT_THREAD(tid) \
    g.ds->deinitThread(tid); \
    __RCU_DEINIT_THREAD; \
    __RLU_DEINIT_THREAD;
#define INIT_ALL \
    __RCU_INIT_ALL; \
    __RLU_INIT_ALL;
#define DEINIT_ALL \
    __RLU_DEINIT_ALL; \
    __RCU_DEINIT_ALL;

#if !defined USE_GSTATS
    #error "Must define USE_GSTATS."
#endif

struct globals_t {    
    PAD;
    // const
    void * const NO_VALUE;
    const test_type KEY_MIN;
    const test_type KEY_MAX; // must be less than std::max(), because the snap collector needs a reserved key larger than this!
    const long long PREFILL_INTERVAL_MILLIS;
    PAD;
    // write once
    long elapsedMillis;
    long long prefillKeySum;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    PAD;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    long long startClockTicks;
    PAD;
    long elapsedMillisNapping;
    volatile long long prefillIntervalElapsedMillis;
    PAD;
    volatile test_type garbage; // used to prevent optimizing out some code
    PAD;
    ds_adapter<test_type, VALUE_TYPE, RECLAIM<>, ALLOC<>, POOL<> > *ds; // the data structure
    PAD;
    Random rngs[MAX_THREADS_POW2]; // create per-thread random number generators (padded to avoid false sharing)
//    PAD; // not needed because of padding at the end of rngs
    volatile bool start;
    PAD;
    volatile bool done;
    PAD;
    volatile int running; // number of threads that are running
    PAD;
    
    globals_t()
    : NO_VALUE(NULL)
    , KEY_MIN(std::numeric_limits<test_type>::min()+1) 
    , KEY_MAX(std::numeric_limits<test_type>::max()-1)
    , PREFILL_INTERVAL_MILLIS(100)
    {
        start = false;
        done = false;
        running = 0;
        ds = NULL;
        garbage = 0;
        prefillIntervalElapsedMillis = 0;
        prefillKeySum = 0;
    }
} g;

#define PRINTI(name) { std::cout<<#name<<"="<<name<<std::endl; }
#define PRINTS(name) { std::cout<<#name<<"="<<STR(name)<<std::endl; }

#ifndef OPS_BETWEEN_TIME_CHECKS
#define OPS_BETWEEN_TIME_CHECKS 500
#endif
#ifndef RQS_BETWEEN_TIME_CHECKS
#define RQS_BETWEEN_TIME_CHECKS 10
#endif

#define GET_COUNTERS 
#define CLEAR_COUNTERS 
    
void *thread_prefill(void *_id) {
    int tid = *((int*) _id);
    binding_bindThread(tid, LOGICAL_PROCESSORS);
    Random *rng = &g.rngs[tid];
    test_type garbage = 0;

    double insProbability = (INS > 0 ? 100*INS/(INS+DEL) : 50.);
    
    INIT_THREAD(tid);
    __sync_fetch_and_add(&g.running, 1);
    __sync_synchronize();
    while (!g.start) { __sync_synchronize(); TRACE COUTATOMICTID("waiting to start"<<std::endl); } // wait to start
    int cnt = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> __endTime = g.startTime;
    while (!g.done) {
        if (((++cnt) % OPS_BETWEEN_TIME_CHECKS) == 0) {
            __endTime = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(__endTime-g.startTime).count() >= g.PREFILL_INTERVAL_MILLIS) {
                g.done = true;
                __sync_synchronize();
                break;
            }
        }
        
        VERBOSE if (cnt&&((cnt % 1000000) == 0)) COUTATOMICTID("op# "<<cnt<<std::endl);
        int key = rng->nextNatural(MAXKEY);
        double op = rng->nextNatural(100000000) / 1000000.;
        GSTATS_TIMER_RESET(tid, timer_latency);
        if (op < insProbability) {
            if (g.ds->INSERT_FUNC(tid, key, KEY_TO_VALUE(key)) == g.ds->getNoValue()) {
                GSTATS_ADD(tid, key_checksum, key);
                GSTATS_ADD(tid, prefill_size, 1);
            }
            GSTATS_ADD(tid, num_updates, 1);
        } else {
            if (g.ds->erase(tid, key) != g.ds->getNoValue()) {
                GSTATS_ADD(tid, key_checksum, -key);
                GSTATS_ADD(tid, prefill_size, -1);
            }
            GSTATS_ADD(tid, num_updates, 1);
        }
        GSTATS_ADD(tid, num_operations, 1);
        GSTATS_TIMER_APPEND_ELAPSED(tid, timer_latency, latency_updates);
    }

    __sync_fetch_and_add(&g.running, -1);
    while (g.running) {
        // wait
    }
    
    DEINIT_THREAD(tid);
    g.garbage += garbage;
    __sync_bool_compare_and_swap(&g.prefillIntervalElapsedMillis, 0, std::chrono::duration_cast<std::chrono::milliseconds>(__endTime-g.startTime).count());
    pthread_exit(NULL);
}

void prefill() {
    std::chrono::time_point<std::chrono::high_resolution_clock> prefillStartTime = std::chrono::high_resolution_clock::now();

    const double PREFILL_THRESHOLD = 0.01;
    const int MAX_ATTEMPTS = 1000;
    const double expectedFullness = (INS+DEL ? INS / (double)(INS+DEL) : 0.5); // percent full in expectation
    const int expectedSize = (int)(MAXKEY * expectedFullness);

    long long totalThreadsPrefillElapsedMillis = 0;
    
    int sz = 0;
    int attempts;
    for (attempts=0;attempts<MAX_ATTEMPTS;++attempts) {
        INIT_ALL;
        
        // create threads
        pthread_t *threads = new pthread_t[TOTAL_THREADS];
        int *ids = new int[TOTAL_THREADS];
        for (int i=0;i<TOTAL_THREADS;++i) {
            ids[i] = i;
            g.rngs[i].setSeed(rand());
        }

        // start all threads
        for (int i=0;i<TOTAL_THREADS;++i) {
            if (pthread_create(&threads[i], NULL, thread_prefill, &ids[i])) {
                std::cerr<<"ERROR: could not create thread"<<std::endl;
                exit(-1);
            }
        }

        TRACE COUTATOMIC("main thread: waiting for threads to START prefilling running="<<g.running<<std::endl);
        while (g.running < TOTAL_THREADS) {}
        TRACE COUTATOMIC("main thread: starting prefilling timer..."<<std::endl);
        g.startTime = std::chrono::high_resolution_clock::now();
        
        g.prefillIntervalElapsedMillis = 0;
        __sync_synchronize();
        g.start = true;
        
        /**
         * START INFINITE LOOP DETECTION CODE
         */
        // amount of time for main thread to wait for children threads
        timespec tsExpected;
        tsExpected.tv_sec = 0;
        tsExpected.tv_nsec = g.PREFILL_INTERVAL_MILLIS * ((__syscall_slong_t) 1000000);
        // short nap
        timespec tsNap;
        tsNap.tv_sec = 0;
        tsNap.tv_nsec = 10000000; // 10ms

        nanosleep(&tsExpected, NULL);
        g.done = true;
        __sync_synchronize();

        const long MAX_NAPPING_MILLIS = 5000;
        g.elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - g.startTime).count();
        g.elapsedMillisNapping = 0;
        while (g.running > 0 && g.elapsedMillisNapping < MAX_NAPPING_MILLIS) {
            nanosleep(&tsNap, NULL);
            g.elapsedMillisNapping = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - g.startTime).count() - g.elapsedMillis;
        }
        if (g.running > 0) {
            COUTATOMIC(std::endl);
            COUTATOMIC("Validation FAILURE: "<<g.running<<" non-responsive thread(s) [during prefill]"<<std::endl);
            COUTATOMIC(std::endl);
            exit(-1);
        }
        /**
         * END INFINITE LOOP DETECTION CODE
         */
        
        TRACE COUTATOMIC("main thread: waiting for threads to STOP prefilling running="<<g.running<<std::endl);
        while (g.running > 0) {}

        for (int i=0;i<TOTAL_THREADS;++i) {
            if (pthread_join(threads[i], NULL)) {
                std::cerr<<"ERROR: could not join prefilling thread"<<std::endl;
                exit(-1);
            }
        }
        
        delete[] threads;
        delete[] ids;

        g.start = false;
        g.done = false;

        sz = GSTATS_OBJECT_NAME.get_sum<long long>(prefill_size);
        if (sz > expectedSize*(1-PREFILL_THRESHOLD)) {
            break;
        } else {
            std::cout << " finished prefilling round with ds size: " << sz << std::endl; 
            std::cout<<"pref_round_size="<<sz<<std::endl;
        }
        
        totalThreadsPrefillElapsedMillis += g.prefillIntervalElapsedMillis;
        DEINIT_ALL;
    }
    
    if (attempts >= MAX_ATTEMPTS) {
        std::cerr<<"ERROR: could not prefill to expected size "<<expectedSize<<". reached size "<<sz<<" after "<<attempts<<" attempts"<<std::endl;
        exit(-1);
    }
    
    std::chrono::time_point<std::chrono::high_resolution_clock> prefillEndTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(prefillEndTime-prefillStartTime).count();

    GSTATS_PRINT;
    const long totalSuccUpdates = GSTATS_GET_STAT_METRICS(num_updates, TOTAL)[0].sum;
    g.prefillKeySum = GSTATS_GET_STAT_METRICS(key_checksum, TOTAL)[0].sum;
    COUTATOMIC("finished prefilling to size "<<sz<<" for expected size "<<expectedSize<<" keysum="<< g.prefillKeySum <<" dskeysum="<<g.ds->getKeyChecksum()<<" dssize="<<g.ds->getSize()<<", performing "<<totalSuccUpdates<<" successful updates in "<<(totalThreadsPrefillElapsedMillis/1000.) /*(elapsed/1000.)*/<<" seconds (total time "<<(elapsed/1000.)<<"s)"<<std::endl);
    std::cout<<"pref_size="<<sz<<std::endl;
    std::cout<<"pref_millis="<<elapsed<<std::endl;
    GSTATS_CLEAR_ALL;
    GSTATS_CLEAR_VAL(timer_epoch_latency, get_server_clock());
    GSTATS_CLEAR_VAL(timer_guard_latency, get_server_clock());
}

void *thread_timed(void *_id) {
    int tid = *((int*) _id);
    binding_bindThread(tid, LOGICAL_PROCESSORS);
    test_type garbage = 0;
    Random *rng = &g.rngs[tid];

    test_type * rqResultKeys = new test_type[RQSIZE+MAX_KEYS_PER_NODE];
    VALUE_TYPE * rqResultValues = new VALUE_TYPE[RQSIZE+MAX_KEYS_PER_NODE];
    
    INIT_THREAD(tid);
    papi_create_eventset(tid);
    __sync_fetch_and_add(&g.running, 1);
    __sync_synchronize();
    while (!g.start) { __sync_synchronize(); TRACE COUTATOMICTID("waiting to start"<<std::endl); } // wait to start
    papi_start_counters(tid);
    int cnt = 0;
    int rq_cnt = 0;
    while (!g.done) {
        if (((++cnt) % OPS_BETWEEN_TIME_CHECKS) == 0 || (rq_cnt % RQS_BETWEEN_TIME_CHECKS) == 0) {
            std::chrono::time_point<std::chrono::high_resolution_clock> __endTime = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(__endTime-g.startTime).count() >= std::abs(MILLIS_TO_RUN)) {
                __sync_synchronize();
                g.done = true;
                __sync_synchronize();
                break;
            }
        }
        
        VERBOSE if (cnt&&((cnt % 1000000) == 0)) COUTATOMICTID("op# "<<cnt<<std::endl);
        int key = rng->nextNatural(MAXKEY);
        double op = rng->nextNatural(100000000) / 1000000.;
        if (op < INS) {
            GSTATS_TIMER_RESET(tid, timer_latency);
            if (g.ds->INSERT_FUNC(tid, key, KEY_TO_VALUE(key)) == g.ds->getNoValue()) {
                GSTATS_ADD(tid, key_checksum, key);
            }
            GSTATS_TIMER_APPEND_ELAPSED(tid, timer_latency, latency_updates);
            GSTATS_ADD(tid, num_updates, 1);
        } else if (op < INS+DEL) {
            GSTATS_TIMER_RESET(tid, timer_latency);
            if (g.ds->erase(tid, key) != g.ds->getNoValue()) {
                GSTATS_ADD(tid, key_checksum, -key);
            }
            GSTATS_TIMER_APPEND_ELAPSED(tid, timer_latency, latency_updates);
            GSTATS_ADD(tid, num_updates, 1);
        } else if (op < INS+DEL+RQ) {
            unsigned _key = rng->nextNatural() % std::max(1, MAXKEY - RQSIZE);
            assert(_key >= 0);
            assert(_key < MAXKEY);
            assert(_key < std::max(1, MAXKEY - RQSIZE));
            assert(MAXKEY > RQSIZE || _key == 0);
            key = (int) _key;
            
            ++rq_cnt;
            int rqcnt;
            GSTATS_TIMER_RESET(tid, timer_latency);
            if (rqcnt = g.ds->rangeQuery(tid, key, key+RQSIZE-1, rqResultKeys, (VALUE_TYPE *) rqResultValues)) {
                garbage += rqResultKeys[0] + rqResultKeys[rqcnt-1]; // prevent rqResultValues and count from being optimized out
            }
            GSTATS_TIMER_APPEND_ELAPSED(tid, timer_latency, latency_rqs);
            GSTATS_ADD(tid, num_rq, 1);
        } else {
            GSTATS_TIMER_RESET(tid, timer_latency);
            if (g.ds->contains(tid, key)) {
            }
            GSTATS_TIMER_APPEND_ELAPSED(tid, timer_latency, latency_searches);
            GSTATS_ADD(tid, num_searches, 1);
        }
        GSTATS_ADD(tid, num_operations, 1);
    }
    __sync_fetch_and_add(&g.running, -1);
    GSTATS_SET(tid, num_prop_thread_exit_time, get_server_clock() - g.startClockTicks);
    while (g.running) { /* wait */ }
    
    papi_stop_counters(tid);
    DEINIT_THREAD(tid);
    delete[] rqResultKeys;
    delete[] rqResultValues;
    g.garbage += garbage;
    pthread_exit(NULL);
}

void *thread_rq(void *_id) {
    int tid = *((int*) _id);
    binding_bindThread(tid, LOGICAL_PROCESSORS);
    test_type garbage = 0;
    Random *rng = &g.rngs[tid];

    test_type * rqResultKeys = new test_type[RQSIZE+MAX_KEYS_PER_NODE];
    VALUE_TYPE * rqResultValues = new VALUE_TYPE[RQSIZE+MAX_KEYS_PER_NODE];
    
    INIT_THREAD(tid);
    papi_create_eventset(tid);
    __sync_fetch_and_add(&g.running, 1);
    __sync_synchronize();
    while (!g.start) { __sync_synchronize(); TRACE COUTATOMICTID("waiting to start"<<std::endl); } // wait to start
    papi_start_counters(tid);
    int cnt = 0;
    while (!g.done) {
        if (((++cnt) % RQS_BETWEEN_TIME_CHECKS) == 0) {
            std::chrono::time_point<std::chrono::high_resolution_clock> __endTime = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(__endTime-g.startTime).count() >= MILLIS_TO_RUN) {
                __sync_synchronize();
                g.done = true;
                __sync_synchronize();
                break;
            }
        }
        
        VERBOSE if (cnt&&((cnt % 1000000) == 0)) COUTATOMICTID("op# "<<cnt<<std::endl);
        unsigned _key = rng->nextNatural() % std::max(1, MAXKEY - RQSIZE);
        assert(_key >= 0);
        assert(_key < MAXKEY);
        assert(_key < std::max(1, MAXKEY - RQSIZE));
        assert(MAXKEY > RQSIZE || _key == 0);
        
        int key = (int) _key;
        int rqcnt;
        GSTATS_TIMER_RESET(tid, timer_latency);
        if (rqcnt = g.ds->rangeQuery(tid, key, key+RQSIZE-1, rqResultKeys, (VALUE_TYPE *) rqResultValues)) {
            garbage += rqResultKeys[0] + rqResultKeys[rqcnt-1]; // prevent rqResultValues and count from being optimized out
        }
        GSTATS_TIMER_APPEND_ELAPSED(tid, timer_latency, latency_rqs);
        GSTATS_ADD(tid, num_rq, 1);
        GSTATS_ADD(tid, num_operations, 1);
    }
    __sync_fetch_and_add(&g.running, -1);
    GSTATS_SET(tid, num_prop_thread_exit_time, get_server_clock() - g.startClockTicks);
    while (g.running) {
        // wait
    }
    
    papi_stop_counters(tid);
    DEINIT_THREAD(tid);
    delete[] rqResultKeys;
    delete[] rqResultValues;
    g.garbage += garbage;
    pthread_exit(NULL);
}

void trial() {
    INIT_ALL;
    papi_init_program(TOTAL_THREADS);
    
    g.ds = new ds_adapter<test_type, VALUE_TYPE, RECLAIM<>, ALLOC<>, POOL<> >(
            TOTAL_THREADS, g.KEY_MIN, g.KEY_MAX, g.NO_VALUE, g.rngs);
    
    // get random number generator seeded with time
    // we use this rng to seed per-thread rng's that use a different algorithm
    srand(time(NULL));

    // create thread data
    pthread_t *threads[TOTAL_THREADS];
    int ids[TOTAL_THREADS];
    for (int i=0;i<TOTAL_THREADS;++i) {
        threads[i] = new pthread_t;
        ids[i] = i;
        g.rngs[i].setSeed(rand());
    }

    DEINIT_ALL;
    
    if (PREFILL) prefill();

    INIT_ALL;

    // amount of time for main thread to wait for children threads
    timespec tsExpected;
    tsExpected.tv_sec = MILLIS_TO_RUN / 1000;
    tsExpected.tv_nsec = (MILLIS_TO_RUN % 1000) * ((__syscall_slong_t) 1000000);
    // short nap
    timespec tsNap;
    tsNap.tv_sec = 0;
    tsNap.tv_nsec = 10000000; // 10ms

    // start all threads
    for (int i=0;i<TOTAL_THREADS;++i) {
        if (pthread_create(threads[i], NULL,
                    (i < WORK_THREADS
                       ? thread_timed
                       : thread_rq), &ids[i])) {
            std::cerr<<"ERROR: could not create thread"<<std::endl;
            exit(-1);
        }
    }

    while (g.running < TOTAL_THREADS) {
        TRACE COUTATOMIC("main thread: waiting for threads to START running="<<g.running<<std::endl);
    } // wait for all threads to be ready
    COUTATOMIC("main thread: starting timer..."<<std::endl);
    
    COUTATOMIC(std::endl);
    COUTATOMIC("###############################################################################"<<std::endl);
    COUTATOMIC("################################ BEGIN RUNNING ################################"<<std::endl);
    COUTATOMIC("###############################################################################"<<std::endl);
    COUTATOMIC(std::endl);
    
    SOFTWARE_BARRIER;
    g.startTime = std::chrono::high_resolution_clock::now();
    g.startClockTicks = get_server_clock();
    __sync_synchronize();
    g.start = true;
    SOFTWARE_BARRIER;

    // pthread_join is replaced with sleeping, and kill threads if they run too long
    // method: sleep for the desired time + a small epsilon,
    //      then check "g.running" to see if we're done.
    //      if not, loop and sleep in small increments for up to 5s,
    //      and exit(-1) if running doesn't hit 0.

    if (MILLIS_TO_RUN > 0) {
        nanosleep(&tsExpected, NULL);
        SOFTWARE_BARRIER;
        g.done = true;
        __sync_synchronize();
    }

    const long MAX_NAPPING_MILLIS = (MILLIS_TO_RUN > 0 ? 5000 : 30000);
    g.elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - g.startTime).count();
    g.elapsedMillisNapping = 0;
    while (g.running > 0 && g.elapsedMillisNapping < MAX_NAPPING_MILLIS) {
        nanosleep(&tsNap, NULL);
        g.elapsedMillisNapping = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - g.startTime).count() - g.elapsedMillis;
    }
    if (g.running > 0) {
        COUTATOMIC(std::endl);
        COUTATOMIC("Validation FAILURE: "<<g.running<<" non-terminating thread(s) [did we exhaust physical memory and experience excessive slowdown due to swap mem?]"<<std::endl);
        COUTATOMIC(std::endl);
        COUTATOMIC("elapsedMillis="<<g.elapsedMillis<<" elapsedMillisNapping="<<g.elapsedMillisNapping<<std::endl);
        
//        for (int i=0;i<TOTAL_THREADS;++i) {
//            pthread_cancel(*(threads[i]));
//        }
        
        if (g.ds->validateStructure()) {
            std::cout<<"Structural validation OK"<<std::endl;
        } else {
            std::cout<<"Structural validation FAILURE."<<std::endl;
        }
        g.ds->printSummary();
#ifdef RQ_DEBUGGING_H
        DEBUG_VALIDATE_RQ(TOTAL_THREADS);
#endif
        exit(-1);
    }

    // join all threads
    for (int i=0;i<TOTAL_THREADS;++i) {
        COUTATOMIC("joining thread "<<i<<std::endl);
        if (pthread_join(*(threads[i]), NULL)) {
            std::cerr<<"ERROR: could not join thread"<<std::endl;
            exit(-1);
        }
    }
    
    COUTATOMIC(std::endl);
    COUTATOMIC("###############################################################################"<<std::endl);
    COUTATOMIC("################################# END RUNNING #################################"<<std::endl);
    COUTATOMIC("###############################################################################"<<std::endl);
    COUTATOMIC(std::endl);
    
    COUTATOMIC(((g.elapsedMillis+g.elapsedMillisNapping)/1000.)<<"s"<<std::endl);

    papi_deinit_program();
    DEINIT_ALL;
   
    for (int i=0;i<TOTAL_THREADS;++i) {
        delete threads[i];
    }
}

void printOutput() {
    std::cout<<"PRODUCING OUTPUT"<<std::endl;

#ifdef USE_GSTATS
    GSTATS_PRINT;
    std::cout<<std::endl;
#endif
    
    long long threadsKeySum = 0;
    
#ifdef USE_GSTATS
    {
        threadsKeySum = GSTATS_GET_STAT_METRICS(key_checksum, TOTAL)[0].sum + g.prefillKeySum;
        long long dsKeySum = g.ds->getKeyChecksum();
        if (threadsKeySum == dsKeySum) {
            std::cout<<"Validation OK: threadsKeySum = "<<threadsKeySum<<" dsKeySum="<<dsKeySum<<std::endl;
        } else {
            std::cout<<"Validation FAILURE: threadsKeySum = "<<threadsKeySum<<" dsKeySum="<<dsKeySum<<std::endl;
            exit(-1);
        }
        std::cout<<"final_keysum="<<threadsKeySum<<std::endl;
    }
#endif
    
    if (g.ds->validateStructure()) {
        std::cout<<"Structural validation OK"<<std::endl;
    } else {
        std::cout<<"Structural validation FAILURE."<<std::endl;
        exit(-1);
    }
    
    long long totalAll = 0;

#ifdef USE_GSTATS
    {
        const long long totalSearches = GSTATS_GET_STAT_METRICS(num_searches, TOTAL)[0].sum;
        const long long totalRQs = GSTATS_GET_STAT_METRICS(num_rq, TOTAL)[0].sum;
        const long long totalQueries = totalSearches + totalRQs;
        const long long totalUpdates = GSTATS_GET_STAT_METRICS(num_updates, TOTAL)[0].sum;

        const double SECONDS_TO_RUN = (MILLIS_TO_RUN)/1000.;
        totalAll = totalUpdates + totalQueries;
        const long long throughputSearches = (long long) (totalSearches / SECONDS_TO_RUN);
        const long long throughputRQs = (long long) (totalRQs / SECONDS_TO_RUN);
        const long long throughputQueries = (long long) (totalQueries / SECONDS_TO_RUN);
        const long long throughputUpdates = (long long) (totalUpdates / SECONDS_TO_RUN);
        const long long throughputAll = (long long) (totalAll / SECONDS_TO_RUN);

        COUTATOMIC(std::endl);
        COUTATOMIC("total_find="<<totalSearches<<std::endl);
        COUTATOMIC("total_rq="<<totalRQs<<std::endl);
        COUTATOMIC("total_updates="<<totalUpdates<<std::endl);
        COUTATOMIC("total_queries="<<totalQueries<<std::endl);
        COUTATOMIC("total_ops="<<totalAll<<std::endl);
        COUTATOMIC("find_throughput="<<throughputSearches<<std::endl);
        COUTATOMIC("rq_throughput="<<throughputRQs<<std::endl);
        COUTATOMIC("update_throughput="<<throughputUpdates<<std::endl);
        COUTATOMIC("query_throughput="<<throughputQueries<<std::endl);
        COUTATOMIC("total_throughput="<<throughputAll<<std::endl);
        COUTATOMIC(std::endl);

        COUTATOMIC(std::endl);
        COUTATOMIC("total find                    : "<<totalSearches<<std::endl);
        COUTATOMIC("total rq                      : "<<totalRQs<<std::endl);
        COUTATOMIC("total updates                 : "<<totalUpdates<<std::endl);
        COUTATOMIC("total queries                 : "<<totalQueries<<std::endl);
        COUTATOMIC("total ops                     : "<<totalAll<<std::endl);
        COUTATOMIC("find throughput               : "<<throughputSearches<<std::endl);
        COUTATOMIC("rq throughput                 : "<<throughputRQs<<std::endl);
        COUTATOMIC("update throughput             : "<<throughputUpdates<<std::endl);
        COUTATOMIC("query throughput              : "<<throughputQueries<<std::endl);
        COUTATOMIC("total throughput              : "<<throughputAll<<std::endl);
        COUTATOMIC(std::endl);
    }
#endif
    
    COUTATOMIC("elapsed milliseconds          : "<<g.elapsedMillis<<std::endl);
    COUTATOMIC("napping milliseconds overtime : "<<g.elapsedMillisNapping<<std::endl);
    COUTATOMIC(std::endl);

    g.ds->printSummary();
    
    // free ds
#ifndef NO_DELETE_DS
    std::cout<<"begin delete ds..."<<std::endl;
    delete g.ds;
    std::cout<<"end delete ds."<<std::endl;
#endif
    
    papi_print_counters(totalAll);
}

int main(int argc, char** argv) {
    
    // setup default args
    PREFILL = false;            // must be false, or else there's no way to specify no prefilling on the command line...
    MILLIS_TO_RUN = 1000;
    RQ_THREADS = 0;
    WORK_THREADS = 4;
    RQSIZE = 0;
    RQ = 0;
    INS = 10;
    DEL = 10;
    MAXKEY = 100000;
    
    // read command line args
    // example args: -i 25 -d 25 -k 10000 -rq 0 -rqsize 1000 -p -t 1000 -nrq 0 -nwork 8
    for (int i=1;i<argc;++i) {
        if (strcmp(argv[i], "-i") == 0) {
            INS = atof(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0) {
            DEL = atof(argv[++i]);
        } else if (strcmp(argv[i], "-rq") == 0) {
            RQ = atof(argv[++i]);
        } else if (strcmp(argv[i], "-rqsize") == 0) {
            RQSIZE = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-k") == 0) {
            MAXKEY = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-nrq") == 0) {
            RQ_THREADS = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-nwork") == 0) {
            WORK_THREADS = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0) {
            MILLIS_TO_RUN = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0) {
            PREFILL = true;
        } else if (strcmp(argv[i], "-pin") == 0) { // e.g., "-pin 1.2.3.8-11.4-7.0"
            binding_parseCustom(argv[++i]); // e.g., "1.2.3.8-11.4-7.0"
            std::cout<<"parsed custom binding: "<<argv[i]<<std::endl;
        } else {
            std::cout<<"bad argument "<<argv[i]<<std::endl;
            exit(1);
        }
    }
    TOTAL_THREADS = WORK_THREADS + RQ_THREADS;
    
    // print used args
    PRINTS(FIND_FUNC);
    PRINTS(INSERT_FUNC);
    PRINTS(ERASE_FUNC);
    PRINTS(RQ_FUNC);
    PRINTS(RECLAIM);
    PRINTS(ALLOC);
    PRINTS(POOL);
    PRINTI(PREFILL);
    PRINTI(MILLIS_TO_RUN);
    PRINTI(INS);
    PRINTI(DEL);
    PRINTI(RQ);
    PRINTI(RQSIZE);
    PRINTI(MAXKEY);
    PRINTI(WORK_THREADS);
    PRINTI(RQ_THREADS);
#ifdef WIDTH_SEQ
    PRINTI(WIDTH_SEQ);
#endif
    
    // print object sizes, to help debugging/sanity checking memory layouts
    g.ds->printObjectSizes();
    
    // setup thread pinning/binding
    binding_configurePolicy(TOTAL_THREADS, LOGICAL_PROCESSORS);
    
    // print actual thread pinning/binding layout
    std::cout<<"ACTUAL_THREAD_BINDINGS=";
    for (int i=0;i<TOTAL_THREADS;++i) {
        std::cout<<(i?",":"")<<binding_getActualBinding(i, LOGICAL_PROCESSORS);
    }
    std::cout<<std::endl;
    if (!binding_isInjectiveMapping(TOTAL_THREADS, LOGICAL_PROCESSORS)) {
        std::cout<<"ERROR: thread binding maps more than one thread to a single logical processor"<<std::endl;
        exit(-1);
    }

    // setup per-thread statistics
    GSTATS_CREATE_ALL;
    
    // initialize a few stat timers to the current time (since i split their values, and want a reasonably recent starting time for the first split)
    GSTATS_CLEAR_VAL(timer_epoch_latency, get_server_clock());
    GSTATS_CLEAR_VAL(timer_guard_latency, get_server_clock());

    trial();
    printOutput();
    
    binding_deinit(LOGICAL_PROCESSORS);
    std::cout<<"garbage="<<g.garbage<<std::endl; // to prevent certain steps from being optimized out
    GSTATS_DESTROY;
    return 0;
}
