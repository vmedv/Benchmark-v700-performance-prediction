/* 
 * File:   multi_counter.h
 * Author: tbrown
 *
 * Created on August 15, 2018, 5:55 PM
 */

#ifndef MULTI_COUNTER_H
#define MULTI_COUNTER_H

#include "plaf.h"
#include "random_fnv1a.h"
#include <cstdlib>

struct SingleCounter {
    union {
        PAD;
        volatile size_t v;
    };
};

class MultiCounter {
private:
    PAD;
    SingleCounter * counters;
    const int numCounters;
    PAD;
public:
    MultiCounter(const int numThreads, const int sizeMultiple)
            : counters(new SingleCounter[std::max(2, sizeMultiple*numThreads)+1]) // allocate one extra entry (don't use first entry---to effectively add padding at the start of the array)
            , numCounters(std::max(2, sizeMultiple*numThreads)) {
        
        counters = counters + 1;                                                // shift by +1 (don't use first entry---to effectively add padding at the start of the array)
        for (int i=0;i<numCounters;++i) {
            counters[i].v = 0;
        }
    }
    ~MultiCounter() {
        delete[] (counters - 1);                                                // shift by -1 (don't use first entry---to effectively add padding at the start of the array)
    }
    inline size_t inc(const int tid, RandomFNV1A * rng, const size_t amt = 1) {
#ifdef USE_LOCAL_HACK
        bool cheap = rng->next(1024) < 512;
        if (cheap) {
            return FAA(&counters[tid].v, amt) + amt;
        }
#endif
        
        const int i = rng->next(numCounters);
        int j;
        do {
            j = rng->next(numCounters);
        } while (i == j);
        size_t vi = counters[i].v;
        size_t vj = counters[j].v;
        return FAA((vi < vj ? &counters[i].v : &counters[j].v), amt) + amt;
    }
    inline size_t readFast(const int tid, RandomFNV1A * rng) {
        const int i = rng->next(numCounters);
        return numCounters * counters[i].v;
    }
    size_t readAccurate() {
        size_t sum = 0;
        for (int i=0;i<numCounters;++i) {
            sum += counters[i].v;
        }
        return sum;
    }
};

#endif /* MULTI_COUNTER_H */

