/*   
 *   File: bst-aravind.c
 *   Author:uthor: Tudor David <tudor.david@epfl.ch>
 *   Description: Aravind Natarajan and Neeraj Mittal. 
 *   Fast Concurrent Lock-free Binary Search Trees. PPoPP 2014
 *   bst-aravind.c is part of ASCYLIB
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
 * File:   wfrbt_ascy.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 10, 2017, 9:41 AM
 */

#ifndef WFRBT_ASCY_H
#define WFRBT_ASCY_H

#if     (INDEX_STRUCT == IDX_WFRBT_ASCY) 
#define SEARCH
#elif   (INDEX_STRUCT == IDX_WFRBT_ASCY_BASELINE)
#define BASELINE
#else
#error
#endif


#define CAS_PTR(a,b,c) __sync_val_compare_and_swap(a,b,c)

#define ALIGNED(N) __attribute__ ((aligned (N)))

#define MEM_BARRIER //nop on the opteron for these benchmarks
#define UNUSED __attribute__ ((unused))
#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })



#define TRUE 1
#define FALSE 0

//MAX_KEY is defined in index_anomaly_bst

#define KEY_MAX  (MAX_KEY - 2)

#define INF2 (KEY_MAX + 2) /*MAX_KEY*/
#define INF1 (KEY_MAX + 1) /*MAX_KEY-1*/
#define INF0 (KEY_MAX) /*MAX_KEY-2*/

//#define MAX_KEY KEY_MAX
//#define MIN_KEY 0

typedef uint8_t bool_t;

template <typename skey_t, typename sval_t>
struct node_t {
    skey_t key;
    sval_t value;
#if defined(BASELINE) || defined(NO_VOLATILE)
    volatile node_t<skey_t, sval_t>* right;
    volatile node_t<skey_t, sval_t>* left;
#else
    node_t * volatile right;
    node_t * volatile left;
#endif 
#ifdef PAD
    char pad[PAD_SIZE];
#endif
#ifdef BASELINE
    uint8_t padding[32];
#endif
};

template <typename skey_t, typename sval_t>
struct seek_record_t {
    node_t<skey_t, sval_t>* ancestor;
    node_t<skey_t, sval_t>* successor;
    node_t<skey_t, sval_t>* parent;
    node_t<skey_t, sval_t>* leaf;
    uint8_t padding[32];
};

template <typename skey_t, typename sval_t, class RecMgr>
class wfrbt_ascy {
private:
    
    node_t<skey_t, sval_t>* root;
    
    node_t<skey_t, sval_t>* initialize_tree();
    node_t<skey_t, sval_t>* create_node(skey_t k, sval_t value, int initializing);
    void bst_seek(skey_t key, node_t<skey_t, sval_t>* node_r, seek_record_t<skey_t, sval_t>* seek_record);
    sval_t bst_search(skey_t key, node_t<skey_t, sval_t>* node_r);
    sval_t bst_insert(skey_t key, sval_t val, node_t<skey_t, sval_t>* node_r);
    sval_t bst_remove(skey_t key, node_t<skey_t, sval_t>* node_r);
    bool_t bst_cleanup(skey_t key, seek_record_t<skey_t, sval_t>* seek_record);
public:

    wfrbt_ascy() {
        root = initialize_tree();
    }

    ~wfrbt_ascy() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        //assert(key < MAX_KEY-1);
        //assert(key < MAX_KEY-2);
        return bst_insert(key, item, root);
    }

    sval_t find(skey_t key) {
        return bst_search(key, root);
    }

    node_t<skey_t, sval_t> * get_root() {
        return (node_t<skey_t, sval_t> *) root->left->left->left;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *) curr->left;
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *) curr->right;
    }
};

#endif /* WFRBT_ASCY_H */

