/* 
 * File:   index_abtree.h
 * Author: trbot
 *
 * Created on May 28, 2017, 6:13 PM
 */

#ifndef INDEX_ABTREE_H
#define INDEX_ABTREE_H

#include <csignal>
#include <cstring>
#include "index_base.h"                                                         // for table_t declaration, and parent class inheritance

/**
 * Define index data structure and record manager types
 */

//#define REBALANCING_NONE
#define USE_SIMPLIFIED_ABTREE_REBALANCING /* a parameter for bslack_impl.h */
#include "bslack_throwaway/bslack_impl.h"

#define DEGREE 16
typedef bslack_Node<DEGREE, KEY_TYPE> NODE_TYPE;
typedef bslack_SCXRecord<DEGREE, KEY_TYPE> DESCRIPTOR_TYPE;

typedef allocator_new<> ALLOCATOR_TYPE;
typedef pool_none<> POOL_TYPE;
#ifdef INDEX_NO_RECLAMATION
typedef reclaimer_none<> RECLAIMER_TYPE;
#else
typedef reclaimer_debra<> RECLAIMER_TYPE;
#endif

typedef record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE, DESCRIPTOR_TYPE> RECORD_MANAGER_TYPE;

typedef bslack<DEGREE, KEY_TYPE, less<KEY_TYPE>, RECORD_MANAGER_TYPE> INDEX_TYPE;   // define this
#define INDEX_CONSTRUCTOR_ARGS MAX_THREADS_POW2, DEGREE, __NO_KEY, SIGQUIT              // define this

/**
 * Create an adapter class for the DBx1000 index interface
 */

class index_abtree : public index_base {
private:
    INDEX_TYPE * index;
    
    unsigned long alignment[9] = {0}; 
    unsigned long sum_nodes_depths = 0;
    unsigned long sum_leaf_depths = 0;
    unsigned long num_leafs = 0;
    unsigned long num_nodes = 0;
    unsigned long num_keys = 0;
    int max_depth = 0;
    
    void calculate_index_stats(NODE_TYPE * curr, int depth) {
        if (curr == NULL) return; 
        unsigned node_alignment = (unsigned long) curr & 63UL;
        assert((node_alignment % 8) == 0);
        alignment[node_alignment/8]++; 
        sum_nodes_depths += depth;
        ++num_nodes;
        if (depth > max_depth) {
            max_depth = depth;
        }
        if (curr->isLeaf()) {
            ++num_leafs;
            num_keys += curr->getABDegree();
            sum_leaf_depths += depth; 
        } else {
            for (int i=0;i<curr->getABDegree();++i) {
                calculate_index_stats(curr->ptrs[i], 1+depth);
            }
        }
    }
public:
    // WARNING: DO NOT OVERLOAD init() WITH NO ARGUMENTS!!!
    RC init(uint64_t part_cnt, table_t * table) {
        if (part_cnt != 1) setbench_error("part_cnt != 1 unsupported");
        index = new INDEX_TYPE(INDEX_CONSTRUCTOR_ARGS);
        this->table = table;
        return RCOK;
    }
    RC index_insert(KEY_TYPE key, VALUE_TYPE newItem, int part_id = -1) {
        newItem->next = NULL;
        lock_key(key);
            void * oldVal = index->insertIfAbsent(tid, key, newItem);
            VALUE_TYPE oldItem = (VALUE_TYPE) oldVal;
            if (oldVal != index->NO_VALUE) {
                // adding to existing list
                newItem->next = oldItem->next;
                oldItem->next = newItem;
            }
        unlock_key(key);
        INCREMENT_NUM_READS(tid);
        return RCOK;
    }
    RC index_read(KEY_TYPE key, VALUE_TYPE * item, int part_id = -1, int thd_id = 0) {
        *item = (VALUE_TYPE) index->find(tid, key).first;
        INCREMENT_NUM_READS(tid);
        return RCOK;
    }
    inline void initThread(const int tid) {
        index->initThread(tid);
    }
    inline void deinitThread(const int tid) {
        index->deinitThread(tid);
    }
    
    size_t getNodeSize() {
        return sizeof(NODE_TYPE);
    }
    
    size_t getDescriptorSize() {
        return sizeof(DESCRIPTOR_TYPE);
    }
    
    void print_stats(){
        calculate_index_stats(index->debug_getEntryPoint(),0);
        cout << "Nodes: "<< num_nodes << endl;
        cout << "Leafs: "<< num_leafs << endl;
        cout << "Keys: "<< num_keys << endl;
        cout << "Node size: " << sizeof(NODE_TYPE) << endl;
        cout << "Descriptor size: " << sizeof(DESCRIPTOR_TYPE) << endl;
        cout << "Max path length: " << max_depth << endl;
        cout << "Avg depth: " << sum_nodes_depths/num_nodes << endl;
        cout << "Avg leaf depth: " << sum_leaf_depths/num_leafs << endl; 
        for (int i=0; i<8 ;i++){
            cout << "Alignment " << i*8 << ": " << (alignment[i]/(double)num_nodes)*100 << "%" << endl;
        }
    }
};

#endif /* INDEX_ABTREE_H */
