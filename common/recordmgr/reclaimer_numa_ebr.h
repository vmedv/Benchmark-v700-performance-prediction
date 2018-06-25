/**
 * C++ record manager implementation (PODC 2015) by Trevor Brown.
 * 
 * Copyright (C) 2015 Trevor Brown
 *
 */

#ifndef RECLAIM_NUMA_EBR_H
#define	RECLAIM_NUMA_EBR_H

#include <cassert>
#include <iostream>
#include <sstream>
#include <limits.h>
#include "blockbag.h"
#include "plaf.h"
#include "allocator_interface.h"
#include "reclaimer_interface.h"

#define EPOCH_INCREMENT 2
#define BITS_EPOCH(ann) ((ann)&~(EPOCH_INCREMENT-1))
#define QUIESCENT(ann) ((ann)&1)
#define GET_WITH_QUIESCENT(ann) ((ann)|1)

#ifdef RAPID_RECLAMATION
#   define MIN_OPS_BEFORE_READ 1
#else
#   define MIN_OPS_BEFORE_READ 20
#endif

#define NUMBER_OF_EPOCH_BAGS 3
#define NUMBER_OF_ALWAYS_EMPTY_EPOCH_BAGS 0

template <typename T = void, class Pool = pool_interface<T> >
class reclaimer_numa_ebr : public reclaimer_interface<T, Pool> {
private:
    class thread_data_t {
    public:
        union {
            char bytes[192];
            struct {
                blockbag<T> * epochbags[NUMBER_OF_EPOCH_BAGS];
                blockbag<T> * currentBag;
                long checked;
                long index;
                long opsSinceRead;
                long timesBagTooLargeSinceRotation;
            };
        };
    };
protected:
//    PAD; // not needed after superclass layout
    volatile long epoch;
    PAD;
    thread_data_t thread_data[MAX_THREADS_POW2];
    std::atomic_long announcedEpoch[MAX_THREADS_POW2*PREFETCH_SIZE_WORDS];   // announcedEpoch[tid*PREFETCH_SIZE_WORDS]
//    PAD; // not needed after padding in preceding var
    
public:
    template<typename _Tp1>
    struct rebind { typedef reclaimer_numa_ebr<_Tp1, Pool> other; };
    template<typename _Tp1, typename _Tp2>
    struct rebind2 { typedef reclaimer_numa_ebr<_Tp1, _Tp2> other; };
    
    inline void getSafeBlockbags(const int tid, blockbag<T> ** bags) {
        SOFTWARE_BARRIER;
        int ix = thread_data[tid].index;
        bags[0] = thread_data[tid].epochbags[ix];
        bags[1] = thread_data[tid].epochbags[((ix+NUMBER_OF_EPOCH_BAGS-1)%NUMBER_OF_EPOCH_BAGS)];
        bags[2] = thread_data[tid].epochbags[((ix+NUMBER_OF_EPOCH_BAGS-2)%NUMBER_OF_EPOCH_BAGS)];
        bags[3] = NULL;
        SOFTWARE_BARRIER;
    }
    
    long long getSizeInNodes() {
        long long sum = 0;
        for (int tid=0;tid<this->NUM_PROCESSES;++tid) {
            for (int j=0;j<NUMBER_OF_EPOCH_BAGS;++j) {
                sum += thread_data[tid].epochbags[j]->computeSize();
            }
        }
        return sum;
    }
    std::string getSizeString() {
        std::stringstream ss;
        ss<<getSizeInNodes(); //<<" in epoch bags";
        return ss.str();
    }
    
    inline static bool quiescenceIsPerRecordType() { return false; }
    inline bool isQuiescent(const int tid) { return QUIESCENT(announcedEpoch[tid*PREFETCH_SIZE_WORDS].load(std::memory_order_relaxed)); }
    inline static bool isProtected(const int tid, T * const obj) { return true; }
    inline static bool isQProtected(const int tid, T * const obj) { return false; }
    inline static bool protect(const int tid, T * const obj, CallbackType notRetiredCallback, CallbackArg callbackArg, bool memoryBarrier = true) { return true; }
    inline static void unprotect(const int tid, T * const obj) {}
    inline static bool qProtect(const int tid, T * const obj, CallbackType notRetiredCallback, CallbackArg callbackArg, bool memoryBarrier = true) { return true; }
    inline static void qUnprotectAll(const int tid) {}
    inline static bool shouldHelp() { return true; }
    
private:
    inline void rotateEpochBags(const int tid) {
        int nextIndex = (thread_data[tid].index+1) % NUMBER_OF_EPOCH_BAGS;
        blockbag<T> * const freeable = thread_data[tid].epochbags[((nextIndex+NUMBER_OF_ALWAYS_EMPTY_EPOCH_BAGS) % NUMBER_OF_EPOCH_BAGS)];
        this->pool->addMoveFullBlocks(tid, freeable); // moves any full blocks (may leave a non-full block behind)
        SOFTWARE_BARRIER;
        thread_data[tid].index = nextIndex;
        thread_data[tid].currentBag = thread_data[tid].epochbags[nextIndex];
    }

public:
    inline void endOp(const int tid) {
        const long ann = announcedEpoch[tid*PREFETCH_SIZE_WORDS].load(std::memory_order_relaxed);
        announcedEpoch[tid*PREFETCH_SIZE_WORDS].store(GET_WITH_QUIESCENT(ann), std::memory_order_relaxed);
    }
    
    inline bool startOp(const int tid, void * const * const reclaimers, const int numReclaimers) {
        SOFTWARE_BARRIER; // prevent any bookkeeping from being moved after this point by the compiler.
        bool result = false;

        long readEpoch = epoch;
        const long ann = announcedEpoch[tid*PREFETCH_SIZE_WORDS].load(std::memory_order_relaxed);

        // if our announced epoch is different from the current epoch
        if (readEpoch != BITS_EPOCH(ann)) {
            // announce the new epoch, and rotate the epoch bags
            thread_data[tid].checked = 0;
            thread_data[tid].timesBagTooLargeSinceRotation = 0;
            for (int i=0;i<numReclaimers;++i) {
                ((reclaimer_numa_ebr<T, Pool> * const) reclaimers[i])->rotateEpochBags(tid);
            }
            result = true;
        }

        // incrementally scan the announced epochs of all threads
        int otherTid = thread_data[tid].checked;
        if ((++thread_data[tid].opsSinceRead % MIN_OPS_BEFORE_READ) == 0) {
            long otherAnnounce = announcedEpoch[otherTid*PREFETCH_SIZE_WORDS].load(std::memory_order_relaxed);
            if (BITS_EPOCH(otherAnnounce) == readEpoch
                    || QUIESCENT(otherAnnounce)) {
                const int c = ++thread_data[tid].checked;
                if (c >= this->NUM_PROCESSES) {
                    __sync_bool_compare_and_swap(&epoch, readEpoch, readEpoch+EPOCH_INCREMENT);
                }
            }
        }
        SOFTWARE_BARRIER;
        if (readEpoch != ann) {
            announcedEpoch[tid*PREFETCH_SIZE_WORDS].store(readEpoch, std::memory_order_relaxed);
        }
        return result;
    }
    
    inline void retire(const int tid, T* p) {
        thread_data[tid].currentBag->add(p);
        DEBUG2 this->debug->addRetired(tid, 1);
        if (thread_data[tid].currentBag->getSizeInBlocks() >= 2) {
            // only execute the following logic once every X times (starting at 0) we see that our current bag is too large
            // (resetting the count when we rotate bags).
            // if we are being prevented from reclaiming often, then we will quickly (within X operations) scan all threads.
            // otherwise, we will avoid scanning all threads too often.
            if ((++thread_data[tid].timesBagTooLargeSinceRotation) % 1000) return;

            long readEpoch = epoch;
            const long ann = announcedEpoch[tid*PREFETCH_SIZE_WORDS].load(std::memory_order_relaxed);

            // if our announced epoch is different from the current epoch, skip the following, since we're going to rotate limbo bags on our next operation, anyway
            if (readEpoch != BITS_EPOCH(ann)) return;
            
            // scan all threads (skipping any threads we've already checked) to see if we can advance the epoch (and subsequently reclaim memory)
            for (; thread_data[tid].checked < this->NUM_PROCESSES; ++thread_data[tid].checked) {
                const int otherTid = thread_data[tid].checked;
                long otherAnnounce = announcedEpoch[otherTid*PREFETCH_SIZE_WORDS].load(std::memory_order_relaxed);
                if (!(BITS_EPOCH(otherAnnounce) == readEpoch || QUIESCENT(otherAnnounce))) return;
            }
            __sync_bool_compare_and_swap(&epoch, readEpoch, readEpoch+EPOCH_INCREMENT);
        }
    }
    
    void debugPrintStatus(const int tid) {
        if (tid == 0) {
            std::cout<<"global_epoch_counter="<<epoch<<std::endl;
        }
    }

    reclaimer_numa_ebr(const int numProcesses, Pool *_pool, debugInfo * const _debug, RecoveryMgr<void *> * const _recoveryMgr = NULL)
            : reclaimer_interface<T, Pool>(numProcesses, _pool, _debug, _recoveryMgr) {
        VERBOSE std::cout<<"constructor reclaimer_numa_ebr helping="<<this->shouldHelp()<<std::endl;// scanThreshold="<<scanThreshold<<std::endl;
        epoch = 0;
        if (numProcesses > MAX_THREADS_POW2) {
            setbench_error("number of threads is greater than MAX_THREADS_POW2 = "<<MAX_THREADS_POW2);
        }
        for (int tid=0;tid<numProcesses;++tid) {
            for (int i=0;i<NUMBER_OF_EPOCH_BAGS;++i) {
                thread_data[tid].epochbags[i] = new blockbag<T>(tid, this->pool->blockpools[tid]);
            }
            thread_data[tid].timesBagTooLargeSinceRotation = 0;
            thread_data[tid].currentBag = thread_data[tid].epochbags[0];
            thread_data[tid].index = 0;
            thread_data[tid].opsSinceRead = 0;
            announcedEpoch[tid*PREFETCH_SIZE_WORDS].store(GET_WITH_QUIESCENT(0), std::memory_order_relaxed);
            thread_data[tid].checked = 0;
        }
    }
    ~reclaimer_numa_ebr() {
        VERBOSE DEBUG std::cout<<"destructor reclaimer_numa_ebr"<<std::endl;
        for (int tid=0;tid<this->NUM_PROCESSES;++tid) {
            for (int i=0;i<NUMBER_OF_EPOCH_BAGS;++i) {
                this->pool->addMoveAll(tid, thread_data[tid].epochbags[i]);
                delete thread_data[tid].epochbags[i];
            }
        }
    }

};

#undef EPOCH_INCREMENT
#undef BITS_EPOCH
#undef QUIESCENT
#undef GET_WITH_QUIESCENT
#undef MIN_OPS_BEFORE_READ
#undef NUMBER_OF_EPOCH_BAGS
#undef NUMBER_OF_ALWAYS_EMPTY_EPOCH_BAGS

#endif

