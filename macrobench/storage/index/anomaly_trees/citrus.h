/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   citrus.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 10, 2017, 11:45 AM
 */

#ifndef CITRUS_H
#define CITRUS_H

#if     (INDEX_STRUCT == IDX_CITRUS_SPIN) 
#define SPIN_LOCK
#define RCU_USE_TSC
#elif   (INDEX_STRUCT == IDX_CITRUS_SPIN_PAD) 
#define SPIN_LOCK 
#define PAD 
#define PAD_SIZE    16 
#define RCU_USE_TSC
#elif   (INDEX_STRUCT == IDX_CITRUS_BASELINE)
#define BASELINE
#define RCU_USE_TSC
#else
#error
#endif

#include "global.h"

#ifdef SPIN_LOCK
typedef pthread_spinlock_t ptlock_t;
#define lock_size               sizeof(ptlock_t)
#define mutex_init(lock)        pthread_spin_init(lock, PTHREAD_PROCESS_PRIVATE)
#define mutex_destroy(lock)     pthread_spin_destroy(lock)
#define mutex_lock(lock)        pthread_spin_lock(lock)
#define mutex_unlock(lock)      pthread_spin_unlock(lock)
#else
typedef pthread_mutex_t ptlock_t;
#define lock_size               sizeof(ptlock_t)
#define mutex_init(lock)        pthread_mutex_init(lock, NULL)
#define mutex_destroy(lock)     pthread_mutex_destroy(lock)
#define mutex_lock(lock)        pthread_mutex_lock(lock)
#define mutex_unlock(lock)      pthread_mutex_unlock(lock)
#endif

template <typename skey_t, typename sval_t>
struct node_t {
    skey_t key;
#if defined(BASELINE) || defined(NO_VOLATILE)
    struct node_t<skey_t, sval_t>* child[2];
#else
    struct node_t<skey_t, sval_t>* volatile child[2];
#endif 

    ptlock_t lock;
    bool marked;
    int tag[2];
    sval_t value;

#ifdef PAD
    char pad[PAD_SIZE];
#endif
};

template <typename skey_t, typename sval_t, class RecMgr>
class citrus {
private:
    node_t<skey_t, sval_t> * root;
    
    node_t<skey_t, sval_t>* newNode(skey_t key, sval_t value);
    node_t<skey_t, sval_t>* init();
    sval_t contains(node_t<skey_t, sval_t>* root, skey_t key);
    bool validate(node_t<skey_t, sval_t>* prev, int tag, node_t<skey_t, sval_t>* curr, int direction);
    sval_t insert(node_t<skey_t, sval_t>* root, skey_t key, sval_t value);
    bool remove(node_t<skey_t, sval_t>* root, skey_t key);
    
public:

    citrus() {
        root = init();
    }

    ~citrus() {
    }

    void initThread(const int tid) { 
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        return insert(root, key, item);
    }

    sval_t find(skey_t key) {
        return contains(root, key);
    }

    node_t<skey_t, sval_t> * get_root() {
        return root;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return curr->child[0];
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return curr->child[1];
    }
};
#endif /* CITRUS_H */

