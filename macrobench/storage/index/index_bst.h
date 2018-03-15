/* 
 * File:   index_bst.h
 * Author: trbot
 *
 * Created on May 28, 2017, 3:03 PM
 */

#ifndef INDEX_BST_H
#define INDEX_BST_H

#include <csignal>
#include <cstring>
#include "index_base.h"     // for table_t declaration, and parent class inheritance

/**
 * Define index data structure and record manager types
 */

#include "bst_throwaway/bst_impl.h"
typedef Node<KEY_TYPE, VALUE_TYPE> NODE_TYPE;
typedef SCXRecord<KEY_TYPE, VALUE_TYPE> DESCRIPTOR_TYPE;

typedef allocator_new<> ALLOCATOR_TYPE;
typedef pool_none<> POOL_TYPE;
#ifdef INDEX_NO_RECLAMATION
typedef reclaimer_none<> RECLAIMER_TYPE;
#else
typedef reclaimer_debra<> RECLAIMER_TYPE;
#endif

typedef record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE, DESCRIPTOR_TYPE> RECORD_MANAGER_TYPE;

typedef bst<KEY_TYPE, VALUE_TYPE, less<KEY_TYPE>, RECORD_MANAGER_TYPE> INDEX_TYPE;
#define INDEX_CONSTRUCTOR_ARGS __NO_KEY, __NO_VALUE, MAX_THREADS_POW2, SIGQUIT



/**
 * Create an adapter class for the DBx1000 index interface
 */

class index_bst : public index_base {
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
        if (((NODE_TYPE *) curr->left.load(std::memory_order_relaxed)) == NULL) {
            ++num_leafs;
            ++num_keys;
            sum_leaf_depths += depth; 
        } else {
            calculate_index_stats((NODE_TYPE *) curr->left.load(std::memory_order_relaxed), 1+depth);
            calculate_index_stats((NODE_TYPE *) curr->right.load(std::memory_order_relaxed), 1+depth);
        }
    }
public:
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
        *item = index->find(tid, key).first;
        INCREMENT_NUM_READS(tid);
        return RCOK;
    }
    void initThread(const int tid) {
        index->initThread(tid);
    }
    void deinitThread(const int tid) {
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

#endif /* INDEX_BST_H */
