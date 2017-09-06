/*   
 *   File: bst-drachsler.c
 *   Author: Tudor David <tudor.david@epfl.ch>
 *   Description: Dana Drachsler, Martin Vechev, and Eran Yahav. 
 *   Practical Concurrent Binary Search Trees via Logical Ordering. PPoPP 2014.
 *   bst-drachsler.c is part of ASCYLIB
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
 * File:   dana.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 3:25 PM
 */

#ifndef DANA_H
#define DANA_H

#if     (INDEX_STRUCT == IDX_DANA_SPIN_FIELDS) 
#define FIELDS_ORDER
#define SPIN_LOCK
#elif   (INDEX_STRUCT == IDX_DANA_SPIN_PAD_FIELDS) 
#define FIELDS_ORDER
#define SPIN_LOCK
#define PAD 
#define PAD_SIZE 24
#elif   (INDEX_STRUCT == IDX_DANA_SPIN_FIELDS_3_LINES)
#define FIELDS_ORDER
#define SPIN_LOCK
#define PAD 
#define PAD_SIZE 120
#elif   (INDEX_STRUCT == IDX_DANA_BASELINE)
#define SPIN_LOCK
#define BASELINE
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
#define TRYLOCK(lock)       pthread_spin_trylock(lock)

#ifdef BASELINE //make sure that small key is defined and no padding
#ifdef PAD
#undef PAD
#endif
#ifdef FIELDS_ORDER
#undef FIELDS_ORDER
#endif 
#endif


#define ALIGNED(N) __attribute__ ((aligned (N)))

#define DRACHSLER_RO_FAIL RO_FAIL

#define TRUE 1
#define FALSE 0



#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


typedef uint8_t bool_t;

template <typename skey_t, typename sval_t>
struct node_t {
  #ifdef FIELDS_ORDER
    skey_t key;
    #ifndef NO_VOLATILE
    node_t<skey_t, sval_t>* volatile left;
    node_t<skey_t, sval_t>* volatile right;
    node_t<skey_t, sval_t>* volatile succ;
    node_t<skey_t, sval_t>* volatile pred;
    #else
    volatile node_t<skey_t, sval_t>* left;
    volatile node_t<skey_t, sval_t>* right;
    volatile node_t<skey_t, sval_t>* succ;
    volatile node_t<skey_t, sval_t>* pred;
    #endif
    bool_t mark;
    sval_t value;
    node_t<skey_t, sval_t>* volatile parent;
    ptlock_t tree_lock;
    ptlock_t succ_lock;
  #else
    volatile node_t<skey_t, sval_t>* left;
    volatile node_t<skey_t, sval_t>* right;
    volatile node_t<skey_t, sval_t>* parent;
    volatile node_t<skey_t, sval_t>* succ;
    volatile node_t<skey_t, sval_t>* pred;
    ptlock_t tree_lock;
    ptlock_t succ_lock;
    skey_t key;
    sval_t value;
    bool_t mark;
  #endif
  #ifdef PAD
    char pad[PAD_SIZE];
  #endif
  #ifdef BASELINE
    char padding[96-5*sizeof(uintptr_t)-2*sizeof(ptlock_t)-sizeof(sval_t)-sizeof(skey_t)-sizeof(bool_t)];
  #endif
};

template <typename skey_t, typename sval_t, class RecMgr>
class dana {
private:
    
    node_t<skey_t, sval_t> * root;
    
    node_t<skey_t, sval_t>* create_node(skey_t k, sval_t value, int initializing);
    node_t<skey_t, sval_t>* initialize_tree();
    node_t<skey_t, sval_t>* bst_search(skey_t key, node_t<skey_t, sval_t>* root);
    sval_t bst_contains(skey_t k, node_t<skey_t, sval_t>* root);
    sval_t bst_insert(skey_t k, sval_t v, node_t<skey_t, sval_t>* root);
    node_t<skey_t, sval_t>* choose_parent(node_t<skey_t, sval_t>* pred, node_t<skey_t, sval_t>* succ, node_t<skey_t, sval_t>* first_cand);
    void insert_to_tree(node_t<skey_t, sval_t>* parent, node_t<skey_t, sval_t>* new_node, node_t<skey_t, sval_t>* root);
    node_t<skey_t, sval_t>* lock_parent(node_t<skey_t, sval_t>* node);
    sval_t bst_remove(skey_t key, node_t<skey_t, sval_t>* root);
    bool_t acquire_tree_locks(node_t<skey_t, sval_t>* n);
    void remove_from_tree(node_t<skey_t, sval_t>* n, bool_t has_two_children, node_t<skey_t, sval_t>* root);
    void update_child(node_t<skey_t, sval_t> *parent, node_t<skey_t, sval_t>* old_ch, node_t<skey_t, sval_t>* new_ch);
    uint32_t bst_size(node_t<skey_t, sval_t>* node);
    
public:
    
    dana() {
        root = initialize_tree();
    }

    ~dana() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        return bst_insert(key, item, root);
    }

    sval_t find(skey_t key) {
        return bst_contains(key, root);
    }

    node_t<skey_t, sval_t> * get_root() {
        return (node_t<skey_t, sval_t> *) root;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *) curr->left;
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *) curr->right;
    }
    
};

#endif /* DANA_H */

