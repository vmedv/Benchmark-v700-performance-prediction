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

#include "gstats_global.h"
GSTATS_DECLARE_STATS_OBJECT(MAX_THREADS_POW2);
GSTATS_DECLARE_ALL_STAT_IDS;

#endif	/* GLOBALS_H */

