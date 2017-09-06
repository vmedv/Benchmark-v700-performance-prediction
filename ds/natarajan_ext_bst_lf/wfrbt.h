/*A Lock Free Binary Search Tree
 
 * File:
 *   wfrbt.cpp
 * Author(s):
 *   Aravind Natarajan <natarajan.aravind@gmail.com>
 * Description:
 *   A Lock Free Binary Search Tree
 *
 * Copyright (c) 20013-2014.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

Please cite our PPoPP 2014 paper - Fast Concurrent Lock-Free Binary Search Trees by Aravind Natarajan and Neeraj Mittal if you use our code in your experiments	

Features:
1. Insert operations directly install their window without injecting the operation into the tree. They help any conflicting operation at the injection point, 
before executing their window txn.
2. Delete operations are the same as that of the original algorithm.
 
 */

/* 
 * File:   wfrbt.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 8, 2017, 10:45 AM
 */

#ifndef WFRBT_H
#define WFRBT_H

#include "atomic_ops.h"



#if     (INDEX_STRUCT == IDX_WFRBT) 
#elif   (INDEX_STRUCT == IDX_WFRBT_BASELINE)
#error cannot support baseline with int keys and no value.  
#else
#error
#endif

// Most of these macros are not used in this algorithm
#define WINDOW_SIZE 10

#define MAX_PROCESSES 63 
#define PROCESS_BITS 6 

#define DELETE_WINDOW_SIZE 5 

#define PTRNODE_RESERVED_BITS 58
#define DATANODE_RESERVED_BITS 55
#define HELPING_THRESHOLD 1000

#define PREGEN_FACTOR 0.75
#define MAX_SLEEP_TIME 100
#define WAITFREE_THRESHOLD 1000
#define FAST_UPDATE_THRESHOLD 1000 // used in phase 3 of update algorithm

#define RECYCLED_VECTOR_RESERVE 5000000

#define GC_NODE_THRESHOLD 1000000

#define MOST_RECENT_COUNT 4

#define MAX_INSERT_DEPTH 9

#define CACHE_LINE_SIZE  64

#define NODE_SIZE 47
#define WORD_SIZE 64

#define WORD_BITS 5
#define KEY_BITS 29

#include <pthread.h>

#define MARK_BIT 1
#define FLAG_BIT 0

#define atomic_cas_full(addr, old_val, new_val) __sync_bool_compare_and_swap(addr, old_val, new_val);
#define create_child_word(addr, mark, flag) (((uintptr_t) addr << 2) + (mark << 1) + (flag))
#define is_marked(x) ( ((x >> 1) & 1)  == 1 ? true:false)
#define is_flagged(x) ( (x & 1 )  == 1 ? true:false)
#define get_addr(x) (x >> 2)
#define add_mark_bit(x) (x + 4UL)
#define is_free(x) (((x) & 3) == 0? true:false)

enum {
    INS, DEL
};

enum {
    BLACK, RED
};

enum {
    UNMARK, MARK
};

enum {
    UNFLAG, FLAG
};

typedef uintptr_t Word;

template <typename skey_t, typename sval_t>
struct node_t {
    skey_t key;
#ifndef BASELINE
    sval_t value;
#endif
    AO_double_t child;
#ifdef PAD
    char pad[PAD_SIZE];
#endif
};

template <typename skey_t, typename sval_t>
struct seekRecord_t {
    skey_t leafKey;
    sval_t leafValue;
    struct node_t<skey_t, sval_t>* parent;
    AO_t pL;
    bool isLeftL; // is L the left child of P?
    struct node_t<skey_t, sval_t>* lum;
    AO_t lumC;
    bool isLeftUM; // is  last unmarked node's child on access path the left child of  the last unmarked node?
};

template <typename skey_t, typename sval_t>
struct thread_data_t {
#ifdef BASELINE
    int id;
    unsigned long numThreads;
    unsigned long numInsert;
    unsigned long numActualDelete;
    unsigned long ops;
    unsigned int seed;
    double search_frac;
    double insert_frac;
    double delete_frac;
    long keyspace1_size;
#endif 
    struct node_t<skey_t, sval_t>* rootOfTree;
    seekRecord_t<skey_t, sval_t>* sr; // seek record
    seekRecord_t<skey_t, sval_t> * ssr; // secondary seek record
};

//static __thread thread_data_t<skey_t, sval_t> * data = NULL;

template <typename skey_t, typename sval_t, class RecMgr>
class wfrbt {
private:
    node_t<skey_t, sval_t> * root;

    seekRecord_t<skey_t, sval_t>* insseek(thread_data_t<skey_t, sval_t>* data, skey_t key, int op);
    seekRecord_t<skey_t, sval_t>* delseek(thread_data_t<skey_t, sval_t>* data, skey_t key, int op);
    seekRecord_t<skey_t, sval_t>* secondary_seek(thread_data_t<skey_t, sval_t>* data, skey_t key, seekRecord_t<skey_t, sval_t>* sr);
    bool delete_node(thread_data_t<skey_t, sval_t>* data, skey_t key);
    sval_t wfrbt_insert(thread_data_t<skey_t, sval_t>* data, skey_t key, sval_t value);
    sval_t search(thread_data_t<skey_t, sval_t>* data, skey_t key);
    int help_conflicting_operation (thread_data_t<skey_t, sval_t>* data, seekRecord_t<skey_t, sval_t>* R);
    int inject(thread_data_t<skey_t, sval_t>* data, seekRecord_t<skey_t, sval_t>* R, int op);
    int perform_one_delete_window_operation(thread_data_t<skey_t, sval_t>* data, seekRecord_t<skey_t, sval_t>* R, skey_t key);
    int perform_one_insert_window_operation(thread_data_t<skey_t, sval_t>* data, seekRecord_t<skey_t, sval_t>* R, skey_t newKey, sval_t value);

public:

    wfrbt() {
        root = (node_t<skey_t, sval_t> *) tree_malloc::allocnode(sizeof (struct node_t<skey_t, sval_t>));
        node_t<skey_t, sval_t> * newLC = (node_t<skey_t, sval_t> *) tree_malloc::allocnode(sizeof (struct node_t<skey_t, sval_t>));
        node_t<skey_t, sval_t> * newRC = (node_t<skey_t, sval_t> *) tree_malloc::allocnode(sizeof (struct node_t<skey_t, sval_t>));

        memset(newLC, 0, sizeof (struct node_t<skey_t, sval_t>));
        memset(newRC, 0, sizeof (struct node_t<skey_t, sval_t>));

        root->key =  MAX_KEY;
        newLC->key = MAX_KEY - 1;
        newRC->key = MAX_KEY;

#ifndef BASELINE
        root->value = NULL;
        newLC->value = NULL;
        newRC->value = NULL;
#endif 

        root->child.AO_val1 = create_child_word(newLC, UNMARK, UNFLAG);
        root->child.AO_val2 = create_child_word(newRC, UNMARK, UNFLAG);
    }

    ~wfrbt() {
    }

    void initThread(const int tid) {
//        data = (thread_data_t<skey_t, sval_t>*) tree_malloc::alloc(sizeof(thread_data_t<skey_t, sval_t>));
//        data->rootOfTree = root;
//        data->sr = (seekRecord_t<skey_t, sval_t>*) tree_malloc::alloc(sizeof(seekRecord_t<skey_t, sval_t>));
//        data->ssr= (seekRecord_t<skey_t, sval_t>*) tree_malloc::alloc(sizeof(seekRecord_t<skey_t, sval_t>));
    }

    void deinitThread(const int tid) {
        //TODO
    }

    sval_t insert(skey_t key, sval_t item) { 
        assert(key < MAX_KEY-1);
        thread_data_t<skey_t, sval_t> data; 
        seekRecord_t<skey_t, sval_t> sr; 
        seekRecord_t<skey_t, sval_t> ssr; 
        data.sr = &sr;
        data.ssr = &ssr;
        data.rootOfTree = root; 
        return wfrbt_insert(&data,key,item);
    }

    sval_t find(skey_t key) {
        thread_data_t<skey_t, sval_t> data; 
        seekRecord_t<skey_t, sval_t> sr; 
        seekRecord_t<skey_t, sval_t> ssr; 
        data.sr = &sr;
        data.ssr = &ssr;
        data.rootOfTree = root;
        return search(&data,key);
    }

    node_t<skey_t, sval_t> * get_root() {
        return root;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *)get_addr(curr->child.AO_val1); 
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *)get_addr(curr->child.AO_val2); 
    }
};
#endif /* WFRBT_H */

