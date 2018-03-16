/* 
 * File:   globals.h
 * Author: trbot
 *
 * Created on March 9, 2015, 1:32 PM
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <string>

#include "plaf.h"

#ifndef SOFTWARE_BARRIER
#define SOFTWARE_BARRIER asm volatile("": : :"memory")
#endif

#include "debugprinting.h"

PAD;
double INS;
double DEL;
double RQ;
int RQSIZE;
int MAXKEY;
int MILLIS_TO_RUN;
bool PREFILL;
int WORK_THREADS;
int RQ_THREADS;
int TOTAL_THREADS;
PAD;

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
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, visited_in_bags, 100, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, skipped_in_bags, 100, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, latency_rqs, 100, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, latency_updates, 100, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, latency_searches, 100, { \
            gstats_output_item(PRINT_HISTOGRAM_LOG, NONE, FULL_DATA) \
      __AND gstats_output_item(PRINT_RAW, SUM, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, AVERAGE, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, STDEV, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MIN, TOTAL) \
      __AND gstats_output_item(PRINT_RAW, MAX, TOTAL) \
    }) \
    gstats_handle_stat(LONG_LONG, skiplist_inserted_on_level, 30, { \
         /* gstats_output_item(PRINT_RAW, NONE, FULL_DATA) */ \
   /* __AND gstats_output_item(PRINT_RAW, SUM, BY_INDEX) */ \
    }) \
    gstats_handle_stat(LONG_LONG, key_checksum, 1, {}) \
    gstats_handle_stat(LONG_LONG, prefill_size, 1, {}) \
    gstats_handle_stat(LONG_LONG, timer_latency, 1, {})

#include "gstats_global.h"
GSTATS_DECLARE_STATS_OBJECT(MAX_THREADS_POW2);
GSTATS_DECLARE_ALL_STAT_IDS;

#endif	/* GLOBALS_H */

