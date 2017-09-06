/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ellen.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 1, 2017, 3:56 PM
 */

#ifndef ELLEN_H
#define ELLEN_H

#include "record_manager.h"
#include <limits.h>

#if (INDEX_STRUCT == IDX_ELLEN_BASELINE)
#define BASELINE
#elif (INDEX_STRUCT == IDX_ELLEN_PAD)
#define PAD
#define PAD_SIZE 16
#else /*INDEX_STRUCT == IDX_ELLEN*/
#endif


/*Other Definitions*/
#define ALIGNED(N) __attribute__ ((aligned (N)))

#define CACHE_LINE_SIZE 64

#if !defined(UNUSED)
#define UNUSED __attribute__ ((unused))
#endif

#define unlikely(x)     __builtin_expect((x), 0)

//atomic operations interface
//Compare-and-swap
#define CAS_PTR(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U8(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U16(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U32(a,b,c) __sync_val_compare_and_swap(a,b,c)
#define CAS_U64(a,b,c) __sync_val_compare_and_swap(a,b,c)

#define MEM_BARRIER //nop on the opteron for these benchmarks

//the states a node can have
//we avoid an enum to better control the size of the data structures
#define STATE_CLEAN 0
#define STATE_DFLAG 1
#define STATE_IFLAG 2
#define STATE_MARK 3

#define TRUE 1
#define FALSE 0



#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

typedef uint8_t leaf_t;
typedef uint8_t bool_t;

#define INF2 MAX_KEY 
#define INF1 (MAX_KEY -1)
//#define MAX_KEY 1073741824 //MAX_KEY should be of the form 2^n-1 for increased random key generation performance

template <typename skey_t, typename sval_t>
union info_t; 

template <typename skey_t, typename sval_t>
struct node_t;

template <typename skey_t, typename sval_t>
struct iinfo_t {
    node_t<skey_t, sval_t>* p;
    node_t<skey_t, sval_t>* new_internal;
    node_t<skey_t, sval_t>* l;
};

template <typename skey_t, typename sval_t>
struct dinfo_t {
    node_t<skey_t, sval_t>* gp;
    node_t<skey_t, sval_t>* p;
    node_t<skey_t, sval_t>* l;
    info_t<skey_t, sval_t>* pupdate;
};

template <typename skey_t, typename sval_t>
union info_t {
    iinfo_t<skey_t, sval_t> iinfo;
    dinfo_t<skey_t, sval_t> dinfo;
    uint8_t padding[CACHE_LINE_SIZE];
};

template <typename skey_t, typename sval_t>
struct node_t {
    skey_t key;
    sval_t value;
    info_t<skey_t, sval_t>* update;
#if defined(BASELINE) || defined(NO_VOLATILE)
    volatile node_t<skey_t, sval_t>* left;
    volatile node_t<skey_t, sval_t>* right;
#else
    node_t<skey_t, sval_t> * volatile left;
    node_t<skey_t, sval_t> * volatile right;
#endif
    bool_t leaf;
#ifdef PAD
    char pad[PAD_SIZE];
#endif
#ifdef BASELINE
    uint8_t padding[CACHE_LINE_SIZE - sizeof (sval_t) - sizeof (skey_t) - sizeof (info_t<skey_t, sval_t>*) - 2 * sizeof (uintptr_t) - sizeof (bool_t)];
#endif
};

template <typename skey_t, typename sval_t>
struct search_result_t {
    node_t<skey_t, sval_t>* gp;
    node_t<skey_t, sval_t>* p;
    node_t<skey_t, sval_t>* l;
    info_t<skey_t, sval_t>* pupdate;
    info_t<skey_t, sval_t>* gpupdate;
#ifdef RESULT_PAD
    char padding[24];
#endif
};

template <typename skey_t, typename sval_t, class RecMgr>
class ellen {
private:
    node_t<skey_t, sval_t> * root;
    node_t<skey_t, sval_t>* create_node(skey_t key, sval_t value, bool_t is_leaf, int initializing);
    int bst_cas_child(node_t<skey_t, sval_t> * parent, node_t<skey_t, sval_t> * old, node_t<skey_t, sval_t> * nnode);
    void bst_help(info_t<skey_t, sval_t>* u);
    void bst_help_marked(info_t<skey_t, sval_t>* op);
    bool_t bst_help_delete(info_t<skey_t, sval_t>* op);
    sval_t bst_delete(skey_t key, node_t<skey_t, sval_t> * root);
    void bst_help_insert(info_t<skey_t, sval_t> * op);
    sval_t bst_insert(skey_t key, sval_t value, node_t<skey_t, sval_t> * root);
    sval_t bst_find(skey_t key, node_t<skey_t, sval_t> * root);
    void bst_search(skey_t key, node_t<skey_t, sval_t> * root, search_result_t<skey_t, sval_t>* resptr);
    info_t<skey_t, sval_t>* create_iinfo_t(node_t<skey_t, sval_t>* p, node_t<skey_t, sval_t>* ni, node_t<skey_t, sval_t>* l);
    info_t<skey_t, sval_t>* create_dinfo_t(node_t<skey_t, sval_t>* gp, node_t<skey_t, sval_t>* p, node_t<skey_t, sval_t>* l, info_t<skey_t, sval_t>* u);
    node_t<skey_t, sval_t> * bst_initialize();
    void bst_init_local();

public:

    ellen() {
        root = bst_initialize();
    }

    ~ellen() {
    }

    void initThread(const int tid) {
        bst_init_local();
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        assert(key < INF1);
        return bst_insert(key, item, root);
    }

    sval_t find(skey_t key) {
        return bst_find(key, root);
    }
    
    node_t<skey_t, sval_t> * get_root(){
        //return root;
        if (root->left->leaf) return NULL;
        return (node_t<skey_t, sval_t> *) root->left->left; 
    }
    
    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr){
        return (node_t<skey_t, sval_t> *) curr->left;
    }
    
    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr){
        return (node_t<skey_t, sval_t> *) curr->right; 
    }
};


#endif /* ELLEN_H */

