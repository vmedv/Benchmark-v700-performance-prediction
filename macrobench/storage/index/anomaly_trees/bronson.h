/*   
 *   File: bst_bronson_java.c
 *   Author: Balmau Oana <oana.balmau@epfl.ch>, 
 *  	     Zablotchi Igor <igor.zablotchi@epfl.ch>, 
 *  	     Tudor David <tudor.david@epfl.ch>
 *   Description: Nathan G. Bronson, Jared Casper, Hassan Chafi
 *   , and Kunle Olukotun. A Practical Concurrent Binary Search Tree. 
 *   PPoPP 2010.
 *   bst_bronson_java.c is part of ASCYLIB
 *
 * Copyright (c) 2014 Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>,
 * 	     	      Tudor David <tudor.david@epfl.ch>
 *	      	      Distributed Programming Lab (LPD), EPFL
 *
 * ASCYLIB is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/* 
 * File:   bronson.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 6, 2017, 1:54 PM
 */

#ifndef BRONSON_H
#define BRONSON_H

#include "record_manager.h"

#if (INDEX_STRUCT == IDX_BRONSON_SPIN)
#define SPIN_LOCK
#elif (INDEX_STRUCT == IDX_BRONSON_SPIN_NO_REREAD) 
#define SPIN_LOCK
#define NO_REREAD
#elif (INDEX_STRUCT == IDX_BRONSON_SPIN_NO_OVL) 
#define SPIN_LOCK
#define NO_OVL
#elif (INDEX_STRUCT == IDX_BRONSON_BASELINE)
#define SPIN_LOCK
#else
#error
#endif

#ifndef SPIN_LOCK
#error only spin lock is currently supported
#endif

typedef pthread_spinlock_t ptlock_t;
#define PTLOCK_SIZE         sizeof(ptlock_t)
#define INIT_LOCK(lock)     pthread_spin_init(lock, PTHREAD_PROCESS_PRIVATE);
#define DESTROY_LOCK(lock)  pthread_spin_destroy(lock)
#define LOCK(lock)          pthread_spin_lock(lock)
#define UNLOCK(lock)        pthread_spin_unlock(lock)

#define MEM_BARRIER //nop on the opteron for these benchmarks
#define UNUSED __attribute__ ((unused))
#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)
/*End Other Definitions*/

#define FOUND -1
#define NOT_FOUND -2
#define RETRY -3

#define UPDATE_IF_PRESENT 1
#define UPDATE_IF_ABSENT 2

#define UNLINKED_OVL 2

#define TRUE 1
#define FALSE 0

#define UNLINK_REQUIRED -1
#define REBALANCE_REQUIRED -2
#define NOTHING_REQUIRED -3

// Spin time for bst_wait_until_not_changing
#define SPIN_COUNT 100

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

typedef uint8_t bool_t;
typedef uint8_t function_t;

template <typename skey_t, typename sval_t>
struct node_t {
#ifdef BASELINE
    volatile int height;
    volatile skey_t key;
    volatile sval_t value;
    volatile uint64_t version;
    node_t * volatile parent;
    node_t * volatile left;
    node_t * volatile right;
    volatile ptlock_t lock;
#else
    volatile skey_t key;
    volatile uint64_t version;
    node_t<skey_t, sval_t> * volatile left;
    node_t<skey_t, sval_t> * volatile right;
    volatile sval_t value;
    node_t<skey_t, sval_t> * volatile parent;
    volatile int height;
    volatile pthread_spinlock_t lock;
#endif   

#ifdef PAD
    char pad[PAD_SIZE];
#endif
};

template <typename skey_t, typename sval_t, class RecMgr>
class bronson {
private:
    node_t<skey_t, sval_t> * volatile root;

    node_t<skey_t, sval_t> * volatile bst_initialize();
    sval_t bst_contains(skey_t k, node_t<skey_t, sval_t> * volatile root);
    sval_t bst_add(skey_t k, sval_t v, node_t<skey_t, sval_t> * volatile root);
    sval_t bst_remove(skey_t k, node_t<skey_t, sval_t> * volatile root);
    void wait_until_not_changing(node_t<skey_t, sval_t> * volatile node);
    bool_t attempt_unlink_nl(node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile node);
    int node_conditon(node_t<skey_t, sval_t> * volatile node);
    void fix_height_and_rebalance(node_t<skey_t, sval_t> * volatile node);
    node_t<skey_t, sval_t> * volatile fix_height_nl(node_t<skey_t, sval_t> * volatile node);
    node_t<skey_t, sval_t> * volatile rebalance_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n);
    node_t<skey_t, sval_t> * volatile rebalance_to_right_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nl, int hr0);
    node_t<skey_t, sval_t> * volatile rebalance_to_left_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nr, int hl0);
    node_t<skey_t, sval_t> * volatile rotate_right_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nl, int hr, int hll, node_t<skey_t, sval_t> * volatile nlr, int hlr);
    node_t<skey_t, sval_t> * volatile rotate_left_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, int hl, node_t<skey_t, sval_t> * volatile nr, node_t<skey_t, sval_t> * volatile nrl, int hrl, int hrr);
    node_t<skey_t, sval_t> * volatile rotate_right_over_left_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nl, int hr, int hll, node_t<skey_t, sval_t> * volatile nlr, int hlrl);
    node_t<skey_t, sval_t> * volatile rotate_left_over_right_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, int hl, node_t<skey_t, sval_t> * volatile nr, node_t<skey_t, sval_t> * volatile nrl, int hrr, int hrlr);
    void set_child(node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile child, bool_t is_right);
    intptr_t attempt_node_update(function_t func, sval_t new_value, node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile node);
    intptr_t attempt_update(skey_t key, function_t func, sval_t new_value, node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile node, uint64_t node_v);
    node_t<skey_t, sval_t> * volatile new_node(int height, skey_t key, uint64_t version, sval_t value, node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile left, node_t<skey_t, sval_t> * volatile right, bool_t initializing);
    bool_t attempt_insert_into_empty(skey_t key, sval_t value, node_t<skey_t, sval_t> * volatile holder);
    intptr_t update_under_root(skey_t k, function_t func, sval_t new_value, node_t<skey_t, sval_t> * volatile holder);
    intptr_t attempt_get(skey_t k, node_t<skey_t, sval_t> * volatile node, bool_t is_right, uint64_t node_v);
    sval_t __contains(node_t<skey_t, sval_t>* curr, skey_t key, uint64_t ovl);

public:

    bronson() {
        root = bst_initialize();
    }

    ~bronson() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        return bst_add(key, item, root);
    }

    sval_t find(skey_t key) {
#if defined(NO_REREAD) || defined(NO_OVL)
        return __contains(root->right, key, root->right->version);
#endif
        return bst_contains(key, root);
    }

    node_t<skey_t, sval_t> * get_root() {
        return (node_t<skey_t, sval_t> *) root;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return curr->left;
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return curr->right;
    }
};

#endif /* BRONSON_H */

