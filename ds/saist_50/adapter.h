#pragma once

#include <iostream>
#include <csignal>
#include <bits/stdc++.h>
using namespace std;

#include "errors.h"
#include "record_manager.h"

#ifdef USE_TREE_STATS
#   include "tree_stats.h"
#endif

#include "saist.h"

#ifndef ALPHA
#define ALPHA 0.5
#endif

#define RECORD_MANAGER_T record_manager<Reclaim, Alloc, Pool, Node<K, V>>
#define DATA_STRUCTURE_T SAIST<K, V>

template<typename K, typename V>
using Node = SAISTNode<K, V>;

template <typename K, typename V, class Reclaim = reclaimer_debra<K>, class Alloc = allocator_new<K>, class Pool = pool_none<K>>
class ds_adapter {
private:
    const V NO_VALUE;
    DATA_STRUCTURE_T * const ds;

public:
    ds_adapter(const int NUM_THREADS,
               const K& KEY_MIN,
               const K& KEY_MAX,
               const V& VALUE_RESERVED,
               Random64 * const unused2)
            : NO_VALUE(VALUE_RESERVED)
            , ds(new DATA_STRUCTURE_T(VALUE_RESERVED, ALPHA, KEY_MIN, KEY_MAX + 1))
    { }

    ~ds_adapter() {
        delete ds;
    }

    V getNoValue() {
        return NO_VALUE;
    }

    void initThread(const int tid) {
//      ds->initThread(tid);
    }
    
    void deinitThread(const int tid) {
//        ds->deinitThread(tid);
    }

    void warmupEnd() {
    }

    V insert(const int tid, const K& key, const V& val) {
        setbench_error("insert-replace functionality not implemented for this data structure");
    }

    V insertIfAbsent(const int tid, const K& key, const V& val) {
        return ds->InsertIfAbsent(key, val);
    }

    V erase(const int tid, const K& key) {
        return ds->Erase(key);
    }

    V find(const int tid, const K& key) {
        return ds->Find(key);
    }

    bool contains(const int tid, const K& key) {
        return ds->Contains(key);
    }

    int rangeQuery(const int tid, const K& lo, const K& hi, K * const resultKeys, V * const resultValues) {
        setbench_error("not implemented");
    }

    void printSummary() {
//        ds->printDebuggingDetails();
//        ds->print_inner_structure();
    }

    bool validateStructure() {
        try {
            ds->Validate();
            return true;
        } catch(const std::runtime_error& e) {
            std::cout << "ERROR WHILE VALIDATING: " << e.what() << '\n';
            return false;
        }
    }

    void printObjectSizes() {
        std::cout<< "sizes: node=" << (sizeof(Node<K, V>)) << std::endl;
    }

#ifdef USE_TREE_STATS
   class NodeHandler {
   public:
       using MyNodeHandler = typename DATA_STRUCTURE_T::NodeHandler;
       using NodePtrType = typename MyNodeHandler::NodePtrType;

    private:
       MyNodeHandler* my_node_handler;

    public:

       NodeHandler(MyNodeHandler* my_node_handler) : my_node_handler(my_node_handler) {
       }

       ~NodeHandler() {
            delete my_node_handler;
       }

       class ChildIterator {
       private:
           using MyChildIterator = typename MyNodeHandler::ChildIterator;

           MyChildIterator my_child_iterator;
       public:
           ChildIterator(MyChildIterator my_child_iterator): my_child_iterator(my_child_iterator) {
           }

           bool hasNext() {
               return my_child_iterator.HasNext();
           }

           NodePtrType next() {
               return my_child_iterator.Next();
           }
       };

       size_t getNumChildren(NodePtrType node) {
           return my_node_handler->GetNumChildren(node);
       }

       bool isLeaf(NodePtrType node) {
           return my_node_handler->IsLeaf(node);
       }

       size_t getNumKeys(NodePtrType node) {
           return my_node_handler->GetNumKeys(node);
       }

       size_t getSumOfKeys(NodePtrType node) {
           return my_node_handler->GetSumKeys(node);
       }

       ChildIterator getChildIterator(NodePtrType node) {
           return ChildIterator(my_node_handler->GetChildIterator(node));
       }
   };

   TreeStats<NodeHandler> * createTreeStats(const K& _minKey, const K& _maxKey) {
       return new TreeStats<NodeHandler>(new NodeHandler(ds->GetNodeHandler()), ds->GetRoot(), false);
   }
#endif
};
