/* 
 * File:   natarajan_ext_bst_lf_adapter.h
 * Author: trbot
 *
 * Created on August 31, 2017, 6:22 PM
 */

#ifndef NATARAJAN_EXT_BST_LF_ADAPTER_H
#define NATARAJAN_EXT_BST_LF_ADAPTER_H

#ifndef STR
    #define STR(x) XSTR(x)
    #define XSTR(x) #x
#endif

#include <iostream>
#ifdef DS_H_FILE
    #include STR(DS_H_FILE)
#else
//    #warning Using default data structure implementation (see define DS_H_FILE)
//    #include "natarajan_ext_bst_lf_baseline_impl.h"
    #include "natarajan_ext_bst_lf_stage2_impl.h"
#endif
#include "errors.h"

#define RECORD_MANAGER_T record_manager<Reclaim, Alloc, Pool, node_t<K, V>>
#define DATA_STRUCTURE_T natarajan_ext_bst_lf<K, V, RECORD_MANAGER_T>

template <class K, class V, class Reclaim, class Alloc, class Pool>
class ds_adapter {
private:
    const V NO_VALUE;
    DATA_STRUCTURE_T * const tree;

public:
    ds_adapter(const K& MIN_KEY, const K& MAX_KEY, const V& _NO_VALUE, const int numThreads, Random * const rngs)
    : NO_VALUE(_NO_VALUE)
    , tree(new DATA_STRUCTURE_T(MAX_KEY, NO_VALUE, numThreads))
    {}
    ~ds_adapter() {
        delete tree;
    }
    
    V getNoValue() {
        return NO_VALUE;
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
        setbench_error("insert-replace not implemented for this data structure");
    }
    V insertIfAbsent(const int tid, const K& key, const V& val) {
        return tree->insertIfAbsent(tid, key, val);
    }
    V erase(const int tid, const K& key) {
    //const std::std::pair<V,bool> erase(const int tid, const K& key) {
        //V retval = tree->erase(tid, key);
        //return std::std::pair<V,bool>(retval, retval != getNoValue());
        return tree->erase(tid, key);
    }
    V find(const int tid, const K& key) {                                  //////////// TODO: USE FIND WITH VALUE RETURN TYPE FOR DBX!!!!
//    //const std::std::pair<V,bool> find(const int tid, const K& key) {
//        //V retval = tree->find(tid, key);
//        //return std::std::pair<V,bool>(retval, retval != getNoValue());
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

#endif
