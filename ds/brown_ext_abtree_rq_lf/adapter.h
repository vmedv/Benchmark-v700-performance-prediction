/**
 * Implementation of a lock-free relaxed (a,b)-tree using LLX/SCX.
 * Trevor Brown, 2018.
 */

#ifndef DS_ADAPTER_H
#define DS_ADAPTER_H

#include <iostream>
#include "errors.h"
#include "random_fnv1a.h"
#include "brown_ext_abtree_lf_impl.h"

#if !defined FAT_NODE_DEGREE
//    #warning "FAT_NODE_DEGREE was not defined... using default: 16."
    #define FAT_NODE_DEGREE 11
#endif

#define RECORD_MANAGER_T record_manager<Reclaim, Alloc, Pool, abtree_ns::Node<FAT_NODE_DEGREE, K>>
#define DATA_STRUCTURE_T abtree_ns::abtree<FAT_NODE_DEGREE, K, std::less<K>, RECORD_MANAGER_T>

template <typename K, typename V, class Reclaim = reclaimer_debra<K>, class Alloc = allocator_new<K>, class Pool = pool_none<K>>
class ds_adapter {
private:
    DATA_STRUCTURE_T * const ds;

public:
    ds_adapter(const int NUM_THREADS,
               const K& KEY_ANY,
               const K& unused1,
               const V& unused2,
               RandomFNV1A * const unused3)
    : ds(new DATA_STRUCTURE_T(NUM_THREADS, KEY_ANY))
    {
        if (!std::is_same<V, void *>::value) {
            setbench_error("Value type V used with brown_ext_abtree_lf is not void *. This data structure has hard writes value type void *.");
        }
        if (NUM_THREADS > MAX_THREADS_POW2) {
            setbench_error("NUM_THREADS exceeds MAX_THREADS_POW2");
        }
    }
    ~ds_adapter() {
        delete ds;
    }
    
    void * getNoValue() {
        return ds->NO_VALUE;
    }
    
    void initThread(const int tid) {
        ds->initThread(tid);
    }
    void deinitThread(const int tid) {
        ds->deinitThread(tid);
    }

    bool contains(const int tid, const K& key) {
        return ds->contains(tid, key);
    }
    void * insert(const int tid, const K& key, void * const val) {
        return ds->insert(tid, key, val);
    }
    void * insertIfAbsent(const int tid, const K& key, void * const val) {
        return ds->insertIfAbsent(tid, key, val);
    }
    void * erase(const int tid, const K& key) {
        return ds->erase(tid, key).first;
    }
    void * find(const int tid, const K& key) {
        return ds->find(tid, key).first;
    }
    int rangeQuery(const int tid, const K& lo, const K& hi, K * const resultKeys, void ** const resultValues) {
        return ds->rangeQuery(tid, lo, hi, resultKeys, resultValues);
    }
    /**
     * Sequential operation to get the number of keys in the set
     */
    int getSize() {
        return ds->getSize();
    }
    void printSummary() {
        std::stringstream ss;
        ss<<ds->getSizeInNodes()<<" nodes in tree";
        std::cout<<ss.str()<<std::endl;
        
        auto recmgr = ds->debugGetRecMgr();
        recmgr->printStatus();
    }
    long long getKeyChecksum() {
        return ds->debugKeySum();
    }
    bool validateStructure() {
        return true;
    }
    void printObjectSizes() {
        std::cout<<"sizes: node="
                 <<(sizeof(abtree_ns::Node<FAT_NODE_DEGREE, K>))
                 <<" descriptor="<<(sizeof(abtree_ns::SCXRecord<FAT_NODE_DEGREE, K>))<<" (statically allocated)"
                 <<std::endl;
    }
};

#undef RECORD_MANAGER_T
#undef DATA_STRUCTURE_T
#undef FAT_NODE_DEGREE

#endif
