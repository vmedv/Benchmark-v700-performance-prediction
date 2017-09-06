/* 
 * File:   bronson_pext_bst_occ_adapter.h
 * Author: trbot
 *
 * Created on September 5, 2017, 10:33 PM
 */

#ifndef BRONSON_PEXT_BST_OCC_ADAPTER_H
#define BRONSON_PEXT_BST_OCC_ADAPTER_H

#include <iostream>
#include "ccavl_impl.h"
#include "errors.h"

#define RECORD_MANAGER_T record_manager<Reclaim, Alloc, Pool, node_t<K, V>>
#define DATA_STRUCTURE_T ccavl<K, V, RECORD_MANAGER_T>

template <class K, class V, class Reclaim, class Alloc, class Pool>
class ds_adapter {
private:
    DATA_STRUCTURE_T * const tree;

public:
    ds_adapter(const K& MIN_KEY, const K& MAX_KEY, const V& _NO_VALUE, const int numThreads, Random * const rngs)
    : tree(new DATA_STRUCTURE_T(numThreads, MIN_KEY))
    {}
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
        error("rangeQuery not implemented for this data structure");
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

#endif
