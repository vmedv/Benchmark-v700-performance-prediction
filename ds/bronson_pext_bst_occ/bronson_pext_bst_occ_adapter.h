/* 
 * File:   bronson_pext_bst_occ_adapter.h
 * Author: trbot
 *
 * Created on September 5, 2017, 10:33 PM
 */

#ifndef BRONSON_PEXT_BST_OCC_ADAPTER_H
#define BRONSON_PEXT_BST_OCC_ADAPTER_H

#include <iostream>
#include "errors.h"
#include "random.h"
#include "ccavl_impl.h"

#define RECORD_MANAGER_T record_manager<Reclaim, Alloc, Pool, node_t<K, V>>
#define DATA_STRUCTURE_T ccavl<K, V, RECORD_MANAGER_T>

template <typename K, typename V, class Reclaim = reclaimer_debra<K>, class Alloc = allocator_new<K>, class Pool = pool_none<K>>
class ds_adapter {
private:
    DATA_STRUCTURE_T * const tree;

public:
    ds_adapter(const int NUM_THREADS,
               const K& KEY_NEG_INFTY,
               const K& unused1,
               const V& unused2,
               Random * const unused3)
    : tree(new DATA_STRUCTURE_T(NUM_THREADS, KEY_NEG_INFTY))
    {
        if (NUM_THREADS > MAX_THREADS_POW2) {
            setbench_error("NUM_THREADS exceeds MAX_THREADS_POW2");
        }
    }
    ~ds_adapter() {
        delete tree;
    }
    
    V getNoValue() {
        return NULL;
    }
    
    void initThread(const int tid) {
        tree->initThread(tid);
    }
    void deinitThread(const int tid) {
        tree->deinitThread(tid);
    }

    bool contains(const int tid, const K& key) {
        return tree->find(tid, key) != getNoValue();
    }
    V insert(const int tid, const K& key, const V& val) {
        return tree->insertReplace(tid, key, val);
    }
    V insertIfAbsent(const int tid, const K& key, const V& val) {
        return tree->insertIfAbsent(tid, key, val);
    }
    V erase(const int tid, const K& key) {
        return tree->erase(tid, key);
    }
    V find(const int tid, const K& key) {
        return tree->find(tid, key);
    }
    int rangeQuery(const int tid, const K& lo, const K& hi, K * const resultKeys, V * const resultValues) {
        setbench_error("rangeQuery not implemented for this data structure");
    }
    /**
     * Sequential operation to get the number of keys in the set
     */
    int getSize() {
        return tree->getSize();
    }

    void printSummary() {
        tree->printSummary();
    }
    long long getKeyChecksum() {
        return tree->getKeyChecksum();
    }
    bool validateStructure() {
        return tree->validateStructure();
    }
    void printObjectSizes() {
        std::cout<<"sizes: node="
                 <<(sizeof(node_t<K, V>))
                 <<std::endl;
    }
};

#undef RECORD_MANAGER_T
#undef DATA_STRUCTURE_T

#endif
