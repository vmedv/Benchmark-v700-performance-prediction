/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   index_anomaly_bst.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 1, 2017, 10:22 AM
 */

#ifndef INDEX_ANOMALY_BST_H
#define INDEX_ANOMALY_BST_H

#include "index_base.h"
#include <limits.h>

#define infinity INT_MAX

typedef KEY_TYPE skey_t;
typedef VALUE_TYPE sval_t;

#define MAX_KEY ULONG_MAX //KEY_MAX
#define MIN_KEY 0

#define ALLOCATOR_TYPE allocator_new<>
#define POOL_TYPE pool_none<>
#define RECLAIMER_TYPE reclaimer_debra<>

#if (INDEX_STRUCT == IDX_HOWLEY) || \
      (INDEX_STRUCT == IDX_HOWLEY_PAD) || \
      (INDEX_STRUCT == IDX_HOWLEY_PAD_LARGE_DES) || \
      (INDEX_STRUCT == IDX_HOWLEY_BASELINE)  
#include "anomaly_trees/howley_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define DESCRIPTOR_TYPE operation_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE, DESCRIPTOR_TYPE>
typedef howley<skey_t, sval_t, void*> INDEX_TYPE;  

#elif (INDEX_STRUCT == IDX_ELLEN) || \
      (INDEX_STRUCT == IDX_ELLEN_PAD) || \
      (INDEX_STRUCT == IDX_ELLEN_BASELINE)
#include "anomaly_trees/ellen_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define DESCRIPTOR_TYPE info_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE, DESCRIPTOR_TYPE>
typedef ellen<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_WFRBT) || \
        (INDEX_STRUCT == IDX_WFRBT_BASELINE)
#include "anomaly_trees/wfrbt_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef wfrbt<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_WFRBT_ASCY) || \
        (INDEX_STRUCT == IDX_WFRBT_ASCY_BASELINE)
#include "anomaly_trees/wfrbt_ascy_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef wfrbt_ascy<skey_t, sval_t, void*> INDEX_TYPE;

#elif   (INDEX_STRUCT == IDX_BRONSON_SPIN) || \
        (INDEX_STRUCT == IDX_BRONSON_SPIN_NO_REREAD) || \
        (INDEX_STRUCT == IDX_BRONSON_SPIN_NO_OVL) || \
        (INDEX_STRUCT == IDX_BRONSON_BASELINE)
#include "anomaly_trees/bronson_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef bronson<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_CCAVL_SPIN) || \
        (INDEX_STRUCT == IDX_CCAVL_SPIN_NO_REREAD) || \
        (INDEX_STRUCT == IDX_CCAVL_SPIN_NO_OVL) || \
        (INDEX_STRUCT == IDX_CCAVL_BASELINE)
#include "anomaly_trees/ccavl_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef ccavl<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_DANA_SPIN_FIELDS) || \
        (INDEX_STRUCT == IDX_DANA_SPIN_PAD_FIELDS) || \
        (INDEX_STRUCT == IDX_DANA_SPIN_FIELDS_3_LINES) || \
        (INDEX_STRUCT == IDX_DANA_BASELINE)
#include "anomaly_trees/dana_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef dana<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_CITRUS_SPIN) || \
        (INDEX_STRUCT == IDX_CITRUS_SPIN_PAD) || \
        (INDEX_STRUCT == IDX_CITRUS_BASELINE)
#include "anomaly_trees/citrus_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef citrus<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_BONSAI) || \
        (INDEX_STRUCT == IDX_BONSAI_PAD) || \
        (INDEX_STRUCT == IDX_BONSAI_BASELINE)
#include "anomaly_trees/bonsai_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef bonsai<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_INTLF) || \
        (INDEX_STRUCT == IDX_INTLF_PAD) || \
        (INDEX_STRUCT == IDX_INTLF_BASELINE)
#include "anomaly_trees/intlf_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef intlf<skey_t, sval_t, void*> INDEX_TYPE;  

#elif   (INDEX_STRUCT == IDX_TICKET) || \
        (INDEX_STRUCT == IDX_TICKET_PAD) || \
        (INDEX_STRUCT == IDX_TICKET_BASELINE) 
#include "anomaly_trees/ticket_impl.h"
#define NODE_TYPE node_t<skey_t, sval_t>
#define RECORD_MANAGER_TYPE record_manager<RECLAIMER_TYPE, ALLOCATOR_TYPE, POOL_TYPE, NODE_TYPE>
typedef ticket<skey_t, sval_t, void*> INDEX_TYPE;  

#else
#error no anomaly tree 
#endif

#if (INDEX_STRUCT == IDX_INTLF) || \
    (INDEX_STRUCT == IDX_INTLF_PAD) || \
    (INDEX_STRUCT == IDX_INTLF_BASELINE)
#define GET_KEY(node) getKey((node)->markAndKey)
#else
#define GET_KEY(node) (node)->key
#endif

/**
 * Create an adapter class for the DBx1000 index interface
 */

class index_anomaly_bst : public index_base {
private:
    INDEX_TYPE * index;
    
    unsigned long alignment[9] = {0}; 
    unsigned long sum_nodes_depths = 0; 
    unsigned long sum_leaf_depths = 0;
    unsigned long num_leafs = 0;  
    unsigned long num_nodes = 0;
    uint64_t leafs_keysum = 0;
    uint64_t nodes_keysum = 0; 
    int max_depth = 0; 
    
    void calculate_index_stats(node_t<skey_t, sval_t> * curr, int depth){
        if (curr == NULL) return; 
        
        skey_t key = GET_KEY(curr);
        if (key != MAX_KEY && key != MIN_KEY){
            //skip dummy nodes .. not perfect
            unsigned node_alignment = (unsigned long)curr & 63UL;
            assert((node_alignment % 8) == 0);
            alignment[node_alignment/8]++; 
            sum_nodes_depths += depth;
            num_nodes++;
            nodes_keysum += key;
        
            if ( index->get_left(curr)==NULL && index->get_right(curr)==NULL ){
              num_leafs++;
              leafs_keysum += key;
              sum_leaf_depths += depth; 
            }
        }
        if( depth > max_depth){
            max_depth = depth;
        }
        calculate_index_stats(index->get_left(curr),depth+1);
        calculate_index_stats(index->get_right(curr),depth+1); 
    }
    
public:
    // WARNING: DO NOT OVERLOAD init() WITH NO ARGUMENTS!!!
    RC init(uint64_t part_cnt, table_t * table) {
        if (part_cnt != 1) setbench_error("part_cnt != 1 unsupported");
        index = new INDEX_TYPE();
        this->table = table;
        return RCOK;
    }
    RC index_insert(KEY_TYPE key, VALUE_TYPE item, int part_id = -1) {
        assert(key < MAX_KEY);
        assert(key > MIN_KEY); //trees might use dummy nodes with min/max keys!
        item->next = NULL;
        lock_key(key);
        VALUE_TYPE oldVal = index->insert(key, item);
        if (oldVal != __NO_VALUE){ //insert returns the first item that was inserted for that key
            item->next = oldVal->next; //insert item to the list of items, after oldVal
            oldVal->next = item; 
        }
        unlock_key(key);
        INCREMENT_NUM_READS(tid);
        return RCOK;
    }
    RC index_read(KEY_TYPE key, VALUE_TYPE * item, int part_id = -1, int thd_id = 0) {
        *item = (VALUE_TYPE) index->find(key);
        INCREMENT_NUM_READS(tid);
        return RCOK;
    }
    inline void initThread(const int tid) {
        index->initThread(tid);
    }
    inline void deinitThread(const int tid) {
        index->deinitThread(tid);
    }
    
    void print_stats(){
        node_t<skey_t, sval_t> * root = index->get_root(); 
        if (num_leafs == 0){
            calculate_index_stats(root,0);
        }
        cout << "Nodes: "<< num_nodes << endl;
        cout << "Leafs: "<< num_leafs << endl;
        cout << "Node size: " << sizeof(node_t<skey_t, sval_t>) << endl;
#ifdef DESCRIPTOR_TYPE
        cout << "Descriptor size: " << sizeof(DESCRIPTOR_TYPE) << endl;
#endif
        cout << "Max path length: " << max_depth << endl;
        cout << "Avg depth: " << sum_nodes_depths/num_nodes << endl;
        cout << "Avg leaf depth: " << sum_leaf_depths/num_leafs << endl; 
        for (int i=0; i<8 ;i++){
            cout << "Alignment " << i*8 << ": " << (alignment[i]/(double)num_nodes)*100 << "%" << endl;
        }
    }
    
    uint64_t getKeySum(){             
        if (nodes_keysum == 0){
            node_t<skey_t, sval_t> * root = index->get_root(); 
            calculate_index_stats(root,0);
        }
        return nodes_keysum;
    }
        
    uint64_t getLeafSum(){             
        if (leafs_keysum == 0){
            node_t<skey_t, sval_t> * root = index->get_root(); 
            calculate_index_stats(root,0);
        }
        return leafs_keysum;
    }  
    
    size_t getNodeSize(){
        return sizeof(node_t<skey_t, sval_t>);
    }
};

#endif /* INDEX_ANOMALY_BST_H */

