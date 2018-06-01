/**
 * Preliminary C++ implementation of binary search tree using LLX/SCX.
 * 
 * Copyright (C) 2014 Trevor Brown
 * This preliminary implementation is CONFIDENTIAL and may not be distributed.
 */

#ifndef RANDOM_H
#define	RANDOM_H

#include "plaf.h"

class RandomFNV1A {
private:
    union {
        unsigned int seed;
        PAD;
    };
public:
    RandomFNV1A(void) {
        this->seed = 0;
    }
    RandomFNV1A(int seed) {
        this->seed = seed;
    }
    
    void setSeed(int seed) {
        this->seed = seed;
    }

    /** returns pseudorandom x satisfying 0 <= x < n. **/
    int next(int n) {
        seed ^= seed << 6;
        seed ^= seed >> 21;
        seed ^= seed << 7;
        int retval = (int) (seed % n);
        return (retval < 0 ? -retval : retval);
    }

    /** returns pseudorandom x satisfying 0 <= x < n. **/
    unsigned int next() {
        seed ^= seed << 6;
        seed ^= seed >> 21;
        seed ^= seed << 7;
        return seed;
    }

};

#endif	/* RANDOM_H */

