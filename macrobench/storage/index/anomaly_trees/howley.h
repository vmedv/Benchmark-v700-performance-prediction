/*   
 *   File: bst_howley.c
 *   Author: Balmau Oana <oana.balmau@epfl.ch>, 
 *  	     Zablotchi Igor <igor.zablotchi@epfl.ch>, 
 *  	     Tudor David <tudor.david@epfl.ch>
 *   Description: Shane V Howley and Jeremy Jones. 
 *   A non-blocking internal binary search tree. SPAA 2012
 *   bst_howley.c is part of ASCYLIB
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
 * File:   howley.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 1, 2017, 10:57 AM
 */

#ifndef HOWLEY_H
#define HOWLEY_H

#include "record_manager.h"
#include <limits.h>

#if (INDEX_STRUCT == IDX_HOWLEY_BASELINE)
#define BASELINE
#elif (INDEX_STRUCT == IDX_HOWLEY_PAD)
#define PAD
#define PAD_SIZE 24
#elif (INDEX_STRUCT == IDX_HOWLEY_PAD_LARGE_DES)
#define LARGE_DES
#define PAD
#define PAD_SIZE 24
#else /*INDEX_STRUCT == IDX_HOWLEY*/
#endif 


/*Other Definitions*/
#define ALIGNED(N) __attribute__ ((aligned (N)))

#define CACHE_LINE_SIZE 64

#if !defined(UNUSED)
#define UNUSED __attribute__ ((unused))
#endif

//atomic operations interface
//Compare-and-swap
#define CAS_PTR(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define BOOL_CAS(a,b,c) __sync_bool_compare_and_swap(a,b,c)
#define CAS_U8(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U16(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U32(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U64(a,b,c) __sync_val_compare_and_swap(a,b,c)

#define TRUE 1
#define FALSE 0

//Encoded in the operation pointer
#define STATE_OP_NONE 0
#define STATE_OP_MARK 1
#define STATE_OP_CHILDCAS 2
#define STATE_OP_RELOCATE 3

//In the relocate_op struct
#define STATE_OP_ONGOING 0
#define STATE_OP_SUCCESSFUL 1
#define STATE_OP_FAILED 2

//States for the result of a search operation
#define FOUND 0x0
#define NOT_FOUND_L 0x1
#define NOT_FOUND_R 0x2
#define ABORT 0x3

#define INF INT_MAX

typedef uint8_t bool_t;

//typedef ALIGNED(64) union operation_t<skey_t,sval_t> operation_t<skey_t,sval_t>;
//typedef ALIGNED(64) struct node_t node_t;

template <typename skey_t, typename sval_t>
struct node_t;

template <typename skey_t, typename sval_t>
union operation_t;

template <typename skey_t, typename sval_t>
struct child_cas_op_t {
    bool_t is_left;
    node_t<skey_t, sval_t>* expected;
    node_t<skey_t, sval_t>* update;

};

template <typename skey_t, typename sval_t>
struct relocate_op_t {
    int /*volatile*/ state; // initialize to ONGOING every time a relocate operation is created
    node_t<skey_t, sval_t>* dest;
    operation_t<skey_t, sval_t>* dest_op;
    skey_t remove_key;
    sval_t remove_value;
    skey_t replace_key;
    sval_t replace_value;

};

template <typename skey_t, typename sval_t>
struct node_t {
    skey_t /*volatile*/ key;
    sval_t value;
    operation_t<skey_t, sval_t>* /*volatile*/ op;
#if defined(BASELINE) || defined(NO_VOLATILE)
    volatile node_t<skey_t, sval_t>* /*volatile*/ left;
    volatile node_t<skey_t, sval_t>* /*volatile*/ right;
#else
    node_t<skey_t, sval_t> * volatile /*volatile*/ left;
    node_t<skey_t, sval_t> * volatile /*volatile*/ right;
#endif 
#ifdef PAD
    char pad[PAD_SIZE];
#endif
#ifdef BASELINE
    uint8_t padding[CACHE_LINE_SIZE - sizeof (sval_t) - sizeof (skey_t) - 3 * sizeof (uintptr_t)];
#endif
};

template <typename skey_t, typename sval_t>
union operation_t {
    child_cas_op_t<skey_t, sval_t> child_cas_op;
    relocate_op_t<skey_t, sval_t> relocate_op;
#ifdef LARGE_DES
    uint8_t padding[2 * CACHE_LINE_SIZE];
#else
    uint8_t padding[CACHE_LINE_SIZE];
#endif
};

template <typename skey_t, typename sval_t, class RecMgr>
class howley {
private:
    node_t<skey_t, sval_t> * root;

    node_t<skey_t, sval_t> * create_node(skey_t key, sval_t value, int initializing);
    operation_t<skey_t, sval_t>* alloc_op();
    sval_t bst_contains(skey_t k, node_t<skey_t, sval_t>* root);
    intptr_t bst_find(skey_t k, node_t<skey_t, sval_t>** pred, operation_t<skey_t, sval_t>** pred_op, node_t<skey_t, sval_t>** curr, operation_t<skey_t, sval_t>** curr_op, node_t<skey_t, sval_t>* aux_root, node_t<skey_t, sval_t>* root);
    sval_t bst_add(skey_t k, sval_t v, node_t<skey_t, sval_t>* root);
    void bst_help_child_cas(operation_t<skey_t, sval_t>* op, node_t<skey_t, sval_t>* dest, node_t<skey_t, sval_t>* root);
    //sval_t bst_remove(skey_t k, node_t<skey_t,sval_t>* root);
    bool_t bst_help_relocate(operation_t<skey_t, sval_t>* op, node_t<skey_t, sval_t>* pred, operation_t<skey_t, sval_t>* pred_op, node_t<skey_t, sval_t>* curr, node_t<skey_t, sval_t>* root);
    void bst_help_marked(node_t<skey_t, sval_t>* pred, operation_t<skey_t, sval_t>* pred_op, node_t<skey_t, sval_t>* curr, node_t<skey_t, sval_t>* root);
    void bst_help(node_t<skey_t, sval_t>* pred, operation_t<skey_t, sval_t>* pred_op, node_t<skey_t, sval_t>* curr, operation_t<skey_t, sval_t>* curr_op, node_t<skey_t, sval_t>* root);

public:

    howley() {
        root = create_node(0, (sval_t) NULL, 1);
    }

    ~howley() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        return bst_add(key, item, root);
    }

    sval_t find(skey_t key) {
        return bst_contains(key, root);
    }
    
    node_t<skey_t, sval_t> * get_root(){
        return root; 
    }
    
    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr){
        return (node_t<skey_t, sval_t> *) curr->left;
    }
    
    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr){
        return (node_t<skey_t, sval_t> *) curr->right; 
    }
};

#endif /* HOWLEY_H */

