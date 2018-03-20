/* 
 * File:   papi_util.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 29, 2016, 3:18 PM
 */

#ifndef PAPI_UTIL_H
#define PAPI_UTIL_H

#ifdef USE_PAPI
#   include <papi.h>
#endif
#include <string>

int all_cpu_counters[] = {
#ifdef USE_PAPI
    PAPI_L1_DCM//,
//    PAPI_L2_TCM,
//    PAPI_L3_TCM,
//    PAPI_RES_STL,
//    PAPI_TOT_CYC,
//    PAPI_TOT_INS //,
#endif
};
std::string all_cpu_counters_strings[] = {
#ifdef USE_PAPI
    "PAPI_L1_DCM"//,
//    "PAPI_L2_TCM",
//    "PAPI_L3_TCM",
//    "PAPI_RES_STL",
//    "PAPI_TOT_CYC",
//    "PAPI_TOT_ISR" //,
//    "PAPI_TLB_DM",
#endif
};
#ifdef USE_PAPI
const int nall_cpu_counters = sizeof(all_cpu_counters) / sizeof(all_cpu_counters[0]);
#endif

void papi_init_program(const int numProcesses);
void papi_deinit_program();
void papi_create_eventset(int id);
void papi_start_counters(int id);
void papi_stop_counters(int id);
void papi_print_counters(long long all_ops);

#endif /* PAPI_UTIL_H */

