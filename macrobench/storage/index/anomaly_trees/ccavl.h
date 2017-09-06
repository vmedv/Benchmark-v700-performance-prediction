//Copyright (c) 2010 Philip W. Howard
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

/* 
 * File:   ccavl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 9:52 AM
 */

#ifndef CCAVL_H
#define CCAVL_H

#if  (INDEX_STRUCT == IDX_CCAVL_SPIN) 
#define SPIN_LOCK
#elif   (INDEX_STRUCT == IDX_CCAVL_SPIN_NO_REREAD)
#define SPIN_LOCK
#define NO_REREAD
#elif   (INDEX_STRUCT == IDX_CCAVL_SPIN_NO_OVL) 
#define SPIN_LOCK
#define NO_OVL
#elif   (INDEX_STRUCT == IDX_CCAVL_BASELINE)
#define BASELINE
//uses pthread mutex 
#else
#error
#endif

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

#define lock_mb() asm volatile("":::"memory")

#define IMPLEMENTED 1
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )
#define ABS(a) ( (a) > 0 ? (a)  : -(a) )

typedef unsigned long long version_t;

template <typename skey_t, typename sval_t>
struct node_t {
#ifndef BASELINE
    skey_t key;
    struct node_t<skey_t, sval_t> * volatile left;
    struct node_t<skey_t, sval_t> * volatile right;
    volatile version_t changeOVL;
    struct node_t * volatile parent;
    sval_t value;
    ptlock_t lock; //note: used to be a pointer to a lock!
    volatile int height;

#ifdef PAD
    char pad[PAD_SIZE];
#endif

#else
    skey_t key;
    sval_t value;
    struct node_t<skey_t, sval_t> * volatile left;
    struct node_t<skey_t, sval_t> * volatile right;
    struct node_t<skey_t, sval_t> * volatile parent;
    unsigned long index;
    long color;
    ptlock_t lock;
    volatile int height;
    volatile version_t changeOVL;
#endif 
};

/** This is a special value that indicates the presence of a null value,
 *  to differentiate from the absence of a value.
 */
static itemid_t t_SpecialNull;
static sval_t SpecialNull = &t_SpecialNull;

/** This is a special value that indicates that an optimistic read
 *  failed.
 */
static itemid_t t_SpecialRetry;
static sval_t SpecialRetry = &t_SpecialRetry;

/** The number of spins before yielding. */
#define SPIN_COUNT 100

/** The number of yields before blocking. */
#define YIELD_COUNT 0

// we encode directions as characters
#define LEFT 'L'
#define RIGHT 'R'

// return type for extreme searches
#define ReturnKey       0
#define ReturnEntry     1
#define ReturnNode      2

#define OVL_BITS_BEFORE_OVERFLOW 8
#define UnlinkedOVL     (1LL)
#define OVLGrowLockMask (2LL)
#define OVLShrinkLockMask (4LL)
#define OVLGrowCountShift (3)
#define OVLShrinkCountShift (OVLGrowCountShift + OVL_BITS_BEFORE_OVERFLOW)
#define OVLGrowCountMask  (((1L << OVL_BITS_BEFORE_OVERFLOW ) - 1) << OVLGrowCountShift)


#define UpdateAlways        0
#define UpdateIfAbsent      1
#define UpdateIfPresent     2
#define UpdateIfEq          3


#define UnlinkRequired          -1
#define RebalanceRequired       -2
#define NothingRequired         -3

template <typename skey_t, typename sval_t, class RecMgr>
class ccavl {
private:
    node_t<skey_t, sval_t> * root;

    node_t<skey_t, sval_t>* rbnode_create(skey_t key, sval_t value, node_t<skey_t, sval_t>* parent);
    sval_t get(node_t<skey_t, sval_t>* tree, skey_t key);
    sval_t put(node_t<skey_t, sval_t>* tree, skey_t key, sval_t value);
    sval_t attemptNodeUpdate(int func,
            sval_t expected,
            sval_t newValue,
            node_t<skey_t, sval_t>* parent,
            node_t<skey_t, sval_t>* curr);
    int attemptUnlink_nl(node_t<skey_t, sval_t>* parent, node_t<skey_t, sval_t>* curr);
    void * remove_node(node_t<skey_t, sval_t>* tree, skey_t key);
    int attemptInsertIntoEmpty(node_t<skey_t, sval_t>* tree, skey_t key, sval_t vOpt);
    sval_t attemptUpdate(
            skey_t key,
            int func,
            sval_t expected,
            sval_t newValue,
            node_t<skey_t, sval_t>* parent,
            node_t<skey_t, sval_t>* curr,
            version_t nodeOVL);
    sval_t update(node_t<skey_t, sval_t>* tree, skey_t key, int func, sval_t expected, sval_t newValue);
    node_t<skey_t, sval_t>* rebalance_nl(node_t<skey_t, sval_t>* nParent, node_t<skey_t, sval_t>* n);
    void fixHeightAndRebalance(node_t<skey_t, sval_t>* curr);
    sval_t __contains(node_t<skey_t,sval_t>* curr, skey_t key, version_t ovl);
public:

    ccavl() {
        root = rbnode_create(LONG_MIN, NULL, NULL);
    }

    ~ccavl() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        return put(root, key, item);
    }

    sval_t find(skey_t key) {

#if defined(NO_REREAD) || defined(NO_OVL)
        return __contains(root->right, key, root->right->changeOVL);
#endif
        return get(root, key);
    }

    node_t<skey_t, sval_t> * get_root() {
        return root;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return curr->left;
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return curr->right;
    }
};

#endif /* CCAVL_H */

