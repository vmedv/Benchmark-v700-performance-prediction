/* 
 * File:   bst_adapter.h
 * Author: trbot
 *
 * Created on August 31, 2017, 6:53 PM
 */

#ifndef BST_ADAPTER_H
#define BST_ADAPTER_H

#include <iostream>
#include "bst_impl.h"
#include "errors.h"
using namespace bst_ns;

#define RECORD_MANAGER_T record_manager<Reclaim, Alloc, Pool, Node<K, V>>
#define DATA_STRUCTURE_T bst<K, V, std::less<K>, RECORD_MANAGER_T>

template <class K, class V, class Reclaim, class Alloc, class Pool>
class ds_adapter {
private:
    const V NO_VALUE;
    DATA_STRUCTURE_T * const ds;

public:
    ds_adapter(const K& MIN_KEY, const K& MAX_KEY, const V& _NO_VALUE, const int numThreads, Random * const rngs)
    : NO_VALUE(_NO_VALUE)
    , ds(new DATA_STRUCTURE_T(MAX_KEY, NO_VALUE, numThreads, SIGQUIT))
    {}
    ~ds_adapter() {
        delete ds;
    }
    
    V getNoValue() {
        return NO_VALUE;
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
    V insert(const int tid, const K& key, const V& val) {
        return ds->insert(tid, key, val);
    }
    V insertIfAbsent(const int tid, const K& key, const V& val) {
        return ds->insertIfAbsent(tid, key, val);
    }
    V erase(const int tid, const K& key) {
        return ds->erase(tid, key).first;
    }
    V find(const int tid, const K& key) {
        return ds->find(tid, key);
    }
    int rangeQuery(const int tid, const K& lo, const K& hi, K * const resultKeys, V * const resultValues) {
        return ds->rangeQuery(tid, lo, hi, resultKeys, resultValues);
    }
    /**
     * Sequential operation to get the number of keys in the set
     */
    int getSize() {
        return ds->getSize();
    }
    void printSummary() {
        stringstream ss;
        ss<<ds->getSizeInNodes()<<" nodes in tree";
        cout<<ss.str()<<endl;
        
        auto recmgr = ds->debugGetRecMgr();
        recmgr->printStatus();
    }
    long long getKeyChecksum() {
        return ds->debugKeySum();
    }
    bool validateStructure() {
        return ds->validate(0, false);
    }
    void printObjectSizes() {
        std::cout<<"sizes: node="
                 <<(sizeof(Node<K, V>))
                 <<" descriptor="<<(sizeof(SCXRecord<K, V>))
                 <<std::endl;
    }
};

#endif
