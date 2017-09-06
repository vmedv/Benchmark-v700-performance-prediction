/*   
 *   File: bst_bronson_java.c
 *   Author: Balmau Oana <oana.balmau@epfl.ch>, 
 *  	     Zablotchi Igor <igor.zablotchi@epfl.ch>, 
 *  	     Tudor David <tudor.david@epfl.ch>
 *   Description: Nathan G. Bronson, Jared Casper, Hassan Chafi
 *   , and Kunle Olukotun. A Practical Concurrent Binary Search Tree. 
 *   PPoPP 2010.
 *   bst_bronson_java.c is part of ASCYLIB
 *qa
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
 * File:   bronson_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 6, 2017, 1:55 PM
 */

#ifndef BRONSON_IMPL_H
#define BRONSON_IMPL_H

#include "bronson.h"

static inline node_t<skey_t, sval_t> * volatile CHILD(node_t<skey_t, sval_t> * volatile parent, bool_t is_right) {
    return is_right ? parent->right : parent->left;
}

static inline uint64_t BEGIN_CHANGE(volatile uint64_t ovl) {
    return (ovl | 1);
}

static inline uint64_t END_CHANGE(volatile uint64_t ovl) {
    return (ovl | 3) + 1;
}

static inline int HEIGHT(node_t<skey_t, sval_t> * volatile node) {
    return node == NULL ? 0 : node->height;
}

static inline bool_t IS_SHRINKING(volatile uint64_t ovl) {
    return (bool_t) ((ovl & 1) != 0);
}

static inline bool_t IS_UNLINKED(volatile uint64_t ovl) {
    return (bool_t) ((ovl & 2) != 0);
}

static inline bool_t IS_SHRINKING_OR_UNLINKED(volatile uint64_t ovl) {
    return (bool_t) ((ovl & 3) != 0L);
}

static inline bool_t SHOULD_UPDATE(function_t func, sval_t prev) {

    return func == UPDATE_IF_ABSENT ? prev == 0 : prev != 0;
}

static inline intptr_t UPDATE_RESULT(function_t func, sval_t prev) {
    if (func == UPDATE_IF_ABSENT) return NOT_FOUND;
    return (intptr_t) prev;
}

static inline intptr_t NO_UPDATE_RESULT(function_t func, sval_t prev) {
    if (func == UPDATE_IF_ABSENT) return (intptr_t) prev;
    return NOT_FOUND;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::bst_initialize() {
    node_t<skey_t, sval_t> * volatile root = new_node(0, 0 /*key*/, 0, 0, NULL, NULL, NULL, TRUE);
    return root;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::new_node(int height, skey_t key, uint64_t version, sval_t value, node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile left, node_t<skey_t, sval_t> * volatile right, bool_t initializing) {

    node_t<skey_t, sval_t> * volatile node;
    node = (node_t<skey_t, sval_t> * volatile) tree_malloc::allocnode(sizeof (node_t<skey_t, sval_t>));

    if (node == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }

    node->height = height;
    node->key = key;
    node->version = version;
    node->value = value;
    node->parent = parent;
    node->left = left;
    node->right = right;
    INIT_LOCK(&(node->lock));

    asm volatile("" :: : "memory");
#ifdef __tile__
    MEM_BARRIER;
#endif
    return node;
}

// When the function returns 0, it means that the node was not found
// (similarly to Howley)

template <typename skey_t, typename sval_t, class RecMgr>
sval_t bronson<skey_t, sval_t, RecMgr>::bst_contains(skey_t key, node_t<skey_t, sval_t> * volatile root) {
    // printf("Bst contains (key %d)\n", key);
    while (TRUE) {
        //PARSE_TRY();
        node_t<skey_t, sval_t> * volatile right = root->right;

        if (right == NULL) {
            // printf("ret1: right==NuLL\n");
            return NULL;
        } else {
            //volatile long right_cmp = key - right->key;

            if (key == right->key) {
                // printf("ret2: right_cmp == 0\n");
                return right->value;
            }

            volatile uint64_t ovl = right->version;
            //if ((ovl & 3)) {
            if (IS_SHRINKING_OR_UNLINKED(ovl)) {
                wait_until_not_changing(right);
            } else if (right == root->right) {
                // if right_cmp < 0, we should go left, otherwise right
                intptr_t vo = attempt_get(key, right, (key < right->key ? FALSE : TRUE), ovl);
                //CHANGE
                if (vo != RETRY) {
                    //return vo == FOUND;
                    if (vo != NOT_FOUND) {
                        // printf("ret3: vo != NOT_FOUND, vo = %d\n", vo);
                        return (sval_t) vo;
                    } else {
                        // printf("ret4: vo != NOT_FOUND, vo = %d\n", vo);
                        return NULL;
                    }
                }
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
intptr_t bronson<skey_t, sval_t, RecMgr>::attempt_get(skey_t k, node_t<skey_t, sval_t> * volatile node, bool_t is_right, uint64_t node_v) {

    // printf("Attempt get: skey %d\n", k);
    while (TRUE) {
        node_t<skey_t, sval_t> * volatile child = CHILD(node, is_right);

        if (child == NULL) {
            if (node->version != node_v) {
                // printf("ret5: node->version != node_v\n");

                return RETRY;
            }

            return NOT_FOUND;
        } else {
            //int child_cmp = k - child->key;

            if (k == child->key) {
                //Verify that it's a value node
                //CHANGE
                //TODO: Leave NOT_FOUND or change to 0?
                // printf("ret6: child_cmp == 0, child->value: %d\n", child->value);

                return child->value ? (intptr_t) child->value : NOT_FOUND;
            }

            uint64_t child_ovl = child->version;
            //if ((child_ovl & 3)){
            if (IS_SHRINKING_OR_UNLINKED(child_ovl)) {
                wait_until_not_changing(child);

                if (node->version != node_v) {
                    // printf("ret7: node->version != node_v\n");

                    return RETRY;
                }
            } else if (child != CHILD(node, is_right)) {
                if (node->version != node_v) {
                    // printf("ret8: node->version != node_v\n");

                    return RETRY;
                }
            } else {
                if (node->version != node_v) {
                    // printf("ret9: node->version != node_v\n");

                    return RETRY;
                }

                intptr_t result = attempt_get(k, child, (k < child->key ? FALSE : TRUE), child_ovl);
                if (result != RETRY) {
                    //CHANGE (leave like this)
                    // printf("ret10: result %d\n", result);

                    return result;
                }
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t bronson<skey_t, sval_t, RecMgr>::bst_add(skey_t key, sval_t v, node_t<skey_t, sval_t> * volatile root) {
    //If something is already present at that particular key, the new value will not be added.
    intptr_t res = update_under_root(key, UPDATE_IF_ABSENT, v, root);
    if (res == NOT_FOUND) return NULL;
    return (sval_t) res;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t bronson<skey_t, sval_t, RecMgr>::bst_remove(skey_t key, node_t<skey_t, sval_t> * volatile root) {
    intptr_t res = update_under_root(key, UPDATE_IF_PRESENT, 0, root);
    return res == NOT_FOUND ? NULL : (sval_t) res;
}

template <typename skey_t, typename sval_t, class RecMgr>
intptr_t bronson<skey_t, sval_t, RecMgr>::update_under_root(skey_t key, function_t func, sval_t new_value, node_t<skey_t, sval_t> * volatile holder) {

    while (TRUE) {
        node_t<skey_t, sval_t> * volatile right = holder->right;

        if (right == NULL) {
            if (!SHOULD_UPDATE(func, 0)) {
                return NO_UPDATE_RESULT(func, 0);
            }

            if (new_value == 0 || attempt_insert_into_empty(key, new_value, holder)) {
                return UPDATE_RESULT(func, 0);
            }
        } else {
            uint64_t ovl = right->version;

            //if ((ovl & 3)){
            if (IS_SHRINKING_OR_UNLINKED(ovl)) {
                wait_until_not_changing(right);
            } else if (right == holder->right) {
                intptr_t vo = attempt_update(key, func, new_value, holder, right, ovl);
                if (vo != RETRY) {
                    // printf("Return from update_under root, vo %d\n", vo);
                    return vo == NOT_FOUND ? 0 : vo;
                }
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool_t bronson<skey_t, sval_t, RecMgr>::attempt_insert_into_empty(skey_t key, sval_t value, node_t<skey_t, sval_t> * volatile holder) {
    skey_t UNUSED holder_key = holder->key;
    
    volatile ptlock_t* holder_lock = &holder->lock;
    LOCK(holder_lock);

    if (holder->right == NULL) {
        holder->right = new_node(1, key, 0, value, holder, NULL, NULL, FALSE);
        holder->height = 2;

        UNLOCK(holder_lock);
        return TRUE;
    } else {

        UNLOCK(holder_lock);
        return FALSE;
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
intptr_t bronson<skey_t, sval_t, RecMgr>::attempt_update(skey_t key, function_t func, sval_t new_value, node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile node, uint64_t node_v) {

    // printf("attempt_update: key %d, new_value %d\n", key, new_value);
    //int cmp = key - node->key;

    if (key == node->key) {
        intptr_t res = attempt_node_update(func, new_value, parent, node);
        return res;
    }

    bool_t is_right = key < node->key ? FALSE : TRUE;

    while (TRUE) {

        node_t<skey_t, sval_t> * volatile child = CHILD(node, is_right);

        if (node->version != node_v) {
            // printf("Retrying 3\n");

            return RETRY;
        }

        if (child == NULL) {

            if (new_value == 0) {

                return NOT_FOUND;
            } else {
                bool_t success;
                node_t<skey_t, sval_t> * volatile damaged;

                {
                    // publish(node);
                    skey_t UNUSED node_key = node->key;
                    volatile ptlock_t* node_lock = &node->lock;
                    LOCK(node_lock);

                    if (node->version != node_v) {
                        // releaseAll();
                        UNLOCK(node_lock);
                        //      f("Retrying 2\n");
                        return RETRY;
                    }

                    if (CHILD(node, is_right) != NULL) {
                        success = FALSE;
                        damaged = NULL;
                    } else {
                        if (!SHOULD_UPDATE(func, 0)) {
                            // releaseAll();
                            UNLOCK(node_lock);

                            return NO_UPDATE_RESULT(func, 0);
                        }

                        node_t<skey_t, sval_t> * volatile new_child = new_node(1, key, 0, new_value, node, NULL, NULL, FALSE);
                        set_child(node, new_child, is_right);

                        success = TRUE;
                        damaged = fix_height_nl(node);
                    }

                    // releaseAll();
                    UNLOCK(node_lock);
                }

                if (success) {
                    fix_height_and_rebalance(damaged);

                    return UPDATE_RESULT(func, 0);
                }
            }

        } else {
            uint64_t child_v = child->version;

            //if ((child_v & 3)){
            if (IS_SHRINKING_OR_UNLINKED(child_v)) {
                wait_until_not_changing(child);
            } else if (child != CHILD(node, is_right)) {
                //RETRY
            } else {
                if (node->version != node_v) {
                    // printf("Retrying 1\n");
                    return RETRY;
                }

                intptr_t vo = attempt_update(key, func, new_value, node, child, child_v);
                // if (vo ==RETRY) printf("Retrying - vo %d\n", vo);
                if (vo != RETRY) {
                    return vo == NOT_FOUND ? 0 : vo;
                }
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
intptr_t bronson<skey_t, sval_t, RecMgr>::attempt_node_update(function_t func, sval_t new_value, node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile node) {


    if (new_value == 0) {
        if (node->value == 0) {

            return NOT_FOUND;
        }
    }

    if (new_value == 0 && (node->left == NULL || node->right == NULL)) {

        sval_t prev;
        node_t<skey_t, sval_t> * volatile damaged;

        {
            // publish(parent);
            // scoped_lock parentLock(parent->lock);
            skey_t UNUSED parent_key = parent->key;
            volatile ptlock_t* parent_lock = &parent->lock;
            LOCK(parent_lock);

            if (IS_UNLINKED(parent->version) || node->parent != parent) {
                // releaseAll();
                UNLOCK(parent_lock);
                return RETRY;
            }

            {
                // publish(node);
                // scoped_lock lock(node->lock);
                skey_t UNUSED node_key = node->key;
                volatile ptlock_t* node_lock = &node->lock;
                LOCK(node_lock);

                prev = node->value;

                if (!SHOULD_UPDATE(func, prev)) {
                    // releaseAll();
                    UNLOCK(node_lock);
                    UNLOCK(parent_lock);
                    return NO_UPDATE_RESULT(func, prev);
                }

                if (prev == 0) {
                    // releaseAll();
                    UNLOCK(node_lock);
                    UNLOCK(parent_lock);
                    return UPDATE_RESULT(func, prev);
                }

                if (!attempt_unlink_nl(parent, node)) {
                    // releaseAll();
                    UNLOCK(node_lock);
                    UNLOCK(parent_lock);
                    return RETRY;
                }

                UNLOCK(node_lock);
            }

            // releaseAll();

            damaged = fix_height_nl(parent);
            UNLOCK(parent_lock);
        }

        fix_height_and_rebalance(damaged);

        return UPDATE_RESULT(func, prev);
    } else {
        // publish(node);
        // scoped_lock lock(node->lock);
        skey_t UNUSED node_key = node->key;
        volatile ptlock_t* node_lock = &node->lock;
        LOCK(node_lock);

        if (IS_UNLINKED(node->version)) {
            // releaseAll();
            UNLOCK(node_lock);
            return RETRY;
        }

        sval_t prev = node->value;
        if (!SHOULD_UPDATE(func, prev)) {
            // releaseAll();
            UNLOCK(node_lock);
            return NO_UPDATE_RESULT(func, prev);
        }

        if (new_value == 0 && (node->left == NULL || node->right == NULL)) {
            // releaseAll();
            UNLOCK(node_lock);
            return RETRY;
        }

        node->value = new_value;

        // releaseAll();
        UNLOCK(node_lock);
        return UPDATE_RESULT(func, prev);
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void bronson<skey_t, sval_t, RecMgr>::wait_until_not_changing(node_t<skey_t, sval_t> * volatile node) {
    //CLEANUP_TRY();

    volatile uint64_t version = node->version;
    int i;

    //if ((version & 1)) {
    if (IS_SHRINKING(version)) {

        for (i = 0; i < SPIN_COUNT; ++i) {
            if (version != node->version) {
                return;
            }
        }

        MEM_BARRIER;
        /* skey_t UNUSED node_key = node->key; */
        /* volatile ptlock_t* node_lock = &node->lock; */
        /* LOCK(node_lock); */
        /* UNLOCK(node_lock); */
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool_t bronson<skey_t, sval_t, RecMgr>::attempt_unlink_nl(node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile node) {

    node_t<skey_t, sval_t> * volatile parent_l = parent->left;
    node_t<skey_t, sval_t> * volatile parent_r = parent->right;

    if (parent_l != node && parent_r != node) {
        return FALSE;
    }

    node_t<skey_t, sval_t> * volatile left = node->left;
    node_t<skey_t, sval_t> * volatile right = node->right;

    if (left != NULL && right != NULL) {

        return FALSE;
    }

    node_t<skey_t, sval_t> * volatile splice = (left != NULL) ? left : right;

    if (parent_l == node) {
        parent->left = splice;
    } else {
        parent->right = splice;
    }

    if (splice != NULL) {
        splice->parent = parent;
    }

    node->version = UNLINKED_OVL;
    node->value = 0;
#ifdef URCU_GC
    free_node(node);
#endif
    // hazard.releaseNode(node);
    return TRUE;
}

template <typename skey_t, typename sval_t, class RecMgr>
int bronson<skey_t, sval_t, RecMgr>::node_conditon(node_t<skey_t, sval_t> * volatile node) {

    node_t<skey_t, sval_t> * volatile nl = node->left;
    node_t<skey_t, sval_t> * volatile nr = node->right;

    if ((nl == NULL || nr == NULL) && node->value == 0) {

        return UNLINK_REQUIRED;
    }

    int hn = node->height;
    int hl0 = HEIGHT(nl);
    int hr0 = HEIGHT(nr);
    int hnrepl = 1 + max(hl0, hr0);
    int bal = hl0 - hr0;

    if (bal < -1 || bal > 1) {

        return REBALANCE_REQUIRED;
    }

    return hn != hnrepl ? hnrepl : NOTHING_REQUIRED;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::fix_height_nl(node_t<skey_t, sval_t> * volatile node) {

    int c = node_conditon(node);

    switch (c) {
        case REBALANCE_REQUIRED:
        case UNLINK_REQUIRED:
            return node;
        case NOTHING_REQUIRED:
            return NULL;
        default:
            node->height = c;
            return node->parent;
    }
}

/*** Beginning of rebalancing functions ***/
template <typename skey_t, typename sval_t, class RecMgr>
void bronson<skey_t, sval_t, RecMgr>::fix_height_and_rebalance(node_t<skey_t, sval_t> * volatile node) {

    while (node != NULL && node->parent != NULL) {


        int condition = node_conditon(node);
        if (condition == NOTHING_REQUIRED || IS_UNLINKED(node->version)) {
            return;
        }

        if (condition != UNLINK_REQUIRED && condition != REBALANCE_REQUIRED) {
            // publish(node);
            // scoped_lock lock(node->lock);

            skey_t UNUSED node_key = node->key;

            volatile ptlock_t* node_lock = &node->lock;
            LOCK(node_lock);

            node = fix_height_nl(node);

            UNLOCK(node_lock);

            // releaseAll();
        } else {

            node_t<skey_t, sval_t> * volatile n_parent = node->parent;
            // publish(n_parent);
            // scoped_lock lock(n_parent->lock);
            //skey_t UNUSED n_parent_key = n_parent->key;
            volatile ptlock_t* n_parent_lock = &n_parent->lock;
            LOCK(n_parent_lock);

            if (!IS_UNLINKED(n_parent->version) && node->parent == n_parent) {
                // publish(node);
                // scoped_lock nodeLock(node->lock);
                skey_t UNUSED node_key = node->key;
                volatile ptlock_t* node_lock = &node->lock;
                LOCK(node_lock);

                node = rebalance_nl(n_parent, node);

                UNLOCK(node_lock);
            }

            UNLOCK(n_parent_lock);
            // releaseAll();
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rebalance_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n) {

    node_t<skey_t, sval_t> * volatile nl = n->left;
    node_t<skey_t, sval_t> * volatile nr = n->right;

    if ((nl == NULL || nr == NULL) && n->value == 0) {
        if (attempt_unlink_nl(n_parent, n)) {
            return fix_height_nl(n_parent);
        } else {
            return n;
        }
    }

    int hn = n->height;
    int hl0 = HEIGHT(nl);
    int hr0 = HEIGHT(nr);
    int hnrepl = 1 + max(hl0, hr0);
    int bal = hl0 - hr0;

    if (bal > 1) {
        return rebalance_to_right_nl(n_parent, n, nl, hr0);
    } else if (bal < -1) {
        return rebalance_to_left_nl(n_parent, n, nr, hl0);
    } else if (hnrepl != hn) {
        n->height = hnrepl;

        return fix_height_nl(n_parent);
    } else {
        return NULL;
    }
}

// checked

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rebalance_to_right_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nl, int hr0) {

    skey_t UNUSED nl_key = nl->key;
    volatile ptlock_t* nl_lock = &nl->lock;
    LOCK(nl_lock);

    int hl = nl->height;
    if (hl - hr0 <= 1) {
        UNLOCK(nl_lock);
        return n;
    } else {
        // publish(nl->right);
        node_t<skey_t, sval_t> * volatile nlr = nl->right;

        int hll0 = HEIGHT(nl->left);
        int hlr0 = HEIGHT(nlr);


        if (hll0 >= hlr0) {
            node_t<skey_t, sval_t> * volatile res = rotate_right_nl(n_parent, n, nl, hr0, hll0, nlr, hlr0);
            UNLOCK(nl_lock);
            return res;
        } else {
            {

                // scoped_lock sublock(nlr->lock);
                skey_t UNUSED nlr_key = nlr->key;
                volatile ptlock_t* nlr_lock = &nlr->lock;
                LOCK(nlr_lock);

                int hlr = nlr->height;
                if (hll0 >= hlr) {
                    node_t<skey_t, sval_t> * volatile res = rotate_right_nl(n_parent, n, nl, hr0, hll0, nlr, hlr);

                    UNLOCK(nlr_lock);
                    UNLOCK(nl_lock);
                    return res;
                } else {
                    int hlrl = HEIGHT(nlr->left);
                    int b = hll0 - hlrl;

                    // CHANGED: Java and C++ implementations differ
                    if (b >= -1 && b <= 1 && !((hll0 == 0 || hlrl == 0) && nl->value == 0)) {
                        node_t<skey_t, sval_t> * volatile res = rotate_right_over_left_nl(n_parent, n, nl, hr0, hll0, nlr, hlrl);
                        UNLOCK(nlr_lock);
                        UNLOCK(nl_lock);
                        return res;
                    }
                }

                // CHANGED
                UNLOCK(nlr_lock);
            }

            node_t<skey_t, sval_t> * volatile res = rebalance_to_left_nl(n, nl, nlr, hll0);
            UNLOCK(nl_lock);
            return res;
        }
    }

}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rebalance_to_left_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nr, int hl0) {


    // publish(nR);
    // scoped_lock lock(nR->lock);

    skey_t UNUSED nr_key = nr->key;
    volatile ptlock_t* nr_lock = &nr->lock;
    LOCK(nr_lock);

    int hr = nr->height;
    if (hl0 - hr >= -1) {
        UNLOCK(nr_lock);
        return n;
    } else {
        node_t<skey_t, sval_t> * volatile nrl = nr->left;
        int hrl0 = HEIGHT(nrl);
        int hrr0 = HEIGHT(nr->right);

        if (hrr0 >= hrl0) {

            node_t<skey_t, sval_t> * volatile res = rotate_left_nl(n_parent, n, hl0, nr, nrl, hrl0, hrr0);
            UNLOCK(nr_lock);
            return res;
        } else {
            {
                // publish(nrl);
                // scoped_lock sublock(nrl->lock);
                skey_t UNUSED nrl_key = nrl->key;
                volatile ptlock_t* nrl_lock = &nrl->lock;
                LOCK(nrl_lock);

                int hrl = nrl->height;
                if (hrr0 >= hrl) {
                    node_t<skey_t, sval_t> * volatile res = rotate_left_nl(n_parent, n, hl0, nr, nrl, hrl, hrr0);
                    UNLOCK(nrl_lock);
                    UNLOCK(nr_lock);
                    return res;
                } else {
                    int hrlr = HEIGHT(nrl->right);
                    int b = hrr0 - hrlr;
                    // CHANGED
                    if (b >= -1 && b <= 1 && !((hrr0 == 0 || hrlr == 0) && nr->value == 0)) {
                        node_t<skey_t, sval_t> * volatile res = rotate_left_over_right_nl(n_parent, n, hl0, nr, nrl, hrr0, hrlr);

                        UNLOCK(nrl_lock);
                        UNLOCK(nr_lock);
                        return res;
                    }
                }

                UNLOCK(nrl_lock);

            }
            node_t<skey_t, sval_t> * volatile res = rebalance_to_right_nl(n, nr, nrl, hrr0);
            UNLOCK(nr_lock);
            return res;
        }
    }

}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rotate_right_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nl, int hr, int hll, node_t<skey_t, sval_t> * volatile nlr, int hlr) {

    uint64_t node_ovl = n->version;
    node_t<skey_t, sval_t> * volatile npl = n_parent->left;
    n->version = BEGIN_CHANGE(node_ovl);

    n->left = nlr;
    if (nlr != NULL) {
        nlr->parent = n;
    }

    nl->right = n;
    n->parent = nl;

    if (npl == n) {
        n_parent->left = nl;
    } else {
        n_parent->right = nl;
    }
    nl->parent = n_parent;

    int hnrepl = 1 + max(hlr, hr);
    n->height = hnrepl;
    nl->height = 1 + max(hll, hnrepl);

    n->version = END_CHANGE(node_ovl);

    int baln = hlr - hr;
    if (baln < -1 || baln > 1) {
        return n;
    }

    // CHANGED 
    if ((nlr == NULL || hr == 0) && n->value == 0) {
        return n;
    }

    int ball = hll - hnrepl;
    if (ball < -1 || ball > 1) {
        return nl;
    }

    // CHANGED
    if (hll == 0 && nl->value == 0) {
        return nl;
    }

    return fix_height_nl(n_parent);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rotate_left_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, int hl, node_t<skey_t, sval_t> * volatile nr, node_t<skey_t, sval_t> * volatile nrl, int hrl, int hrr) {

    uint64_t node_ovl = n->version;
    node_t<skey_t, sval_t> * volatile npl = n_parent->left;
    n->version = BEGIN_CHANGE(node_ovl);

    n->right = nrl;
    if (nrl != NULL) {
        nrl->parent = n;
    }

    nr->left = n;
    n->parent = nr;

    if (npl == n) {
        n_parent->left = nr;
    } else {
        n_parent->right = nr;
    }
    nr->parent = n_parent;

    int hnrepl = 1 + max(hl, hrl);
    n->height = hnrepl;
    nr->height = 1 + max(hnrepl, hrr);

    n->version = END_CHANGE(node_ovl);

    int baln = hrl - hl;
    if (baln < -1 || baln > 1) {
        return n;
    }

    // CHANGED
    if ((nrl == NULL || hl == 0) && n->value == 0) {
        return n;
    }

    int balr = hrr - hnrepl;
    if (balr < -1 || balr > 1) {
        return nr;
    }

    // CHANGED
    if (hrr == 0 && nr->value == 0) {
        return nr;
    }


    return fix_height_nl(n_parent);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rotate_right_over_left_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, node_t<skey_t, sval_t> * volatile nl, int hr, int hll, node_t<skey_t, sval_t> * volatile nlr, int hlrl) {

    uint64_t node_ovl = n->version;
    uint64_t left_ovl = nl->version;

    node_t<skey_t, sval_t> * volatile npl = n_parent->left;
    node_t<skey_t, sval_t> * volatile nlrl = nlr->left;
    node_t<skey_t, sval_t> * volatile nlrr = nlr->right;
    int hlrr = HEIGHT(nlrr);

    n->version = BEGIN_CHANGE(node_ovl);
    nl->version = BEGIN_CHANGE(left_ovl);

    n->left = nlrr;
    if (nlrr != NULL) {
        nlrr->parent = n;
    }

    nl->right = nlrl;
    if (nlrl != NULL) {
        nlrl->parent = nl;
    }

    nlr->left = nl;
    nl->parent = nlr;
    nlr->right = n;
    n->parent = nlr;

    if (npl == n) {
        n_parent->left = nlr;
    } else {
        n_parent->right = nlr;
    }
    nlr->parent = n_parent;

    int hnrepl = 1 + max(hlrr, hr);
    n->height = hnrepl;

    int hlrepl = 1 + max(hll, hlrl);
    nl->height = hlrepl;

    nlr->height = 1 + max(hlrepl, hnrepl);

    n->version = END_CHANGE(node_ovl);
    nl->version = END_CHANGE(left_ovl);

    int baln = hlrr - hr;
    if (baln < -1 || baln > 1) {
        return n;
    }

    if ((nlrr == NULL || hr == 0) && n->value == 0) {
        // repair involves splicing out n and maybe more rotations
        return n;
    }

    int ballr = hlrepl - hnrepl;
    if (ballr < -1 || ballr > 1) {
        return nlr;
    }

    return fix_height_nl(n_parent);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * volatile bronson<skey_t, sval_t, RecMgr>::rotate_left_over_right_nl(node_t<skey_t, sval_t> * volatile n_parent, node_t<skey_t, sval_t> * volatile n, int hl, node_t<skey_t, sval_t> * volatile nr, node_t<skey_t, sval_t> * volatile nrl, int hrr, int hrlr) {

    uint64_t node_ovl = n->version;
    uint64_t right_ovl = nr->version;

    // CHANGED
    n->version = BEGIN_CHANGE(node_ovl);
    nr->version = BEGIN_CHANGE(right_ovl);

    node_t<skey_t, sval_t> * volatile npl = n_parent->left;
    node_t<skey_t, sval_t> * volatile nrll = nrl->left;
    node_t<skey_t, sval_t> * volatile nrlr = nrl->right;
    int hrll = HEIGHT(nrll);


    n->right = nrll;
    if (nrll != NULL) {
        nrll->parent = n;
    }

    nr->left = nrlr;
    if (nrlr != NULL) {
        nrlr->parent = nr;
    }

    nrl->right = nr;
    nr->parent = nrl;
    nrl->left = n;
    n->parent = nrl;

    if (npl == n) {
        n_parent->left = nrl;
    } else {
        n_parent->right = nrl;
    }
    nrl->parent = n_parent;

    int hnrepl = 1 + max(hl, hrll);
    n->height = hnrepl;
    int hrrepl = 1 + max(hrlr, hrr);
    nr->height = hrrepl;
    nrl->height = 1 + max(hnrepl, hrrepl);

    n->version = END_CHANGE(node_ovl);
    nr->version = END_CHANGE(right_ovl);

    int baln = hrll - hl;
    if (baln < -1 || baln > 1) {
        return n;
    }

    // CHANGED
    if ((nrll == NULL || hl == 0) && n->value == 0) {
        return n;
    }

    int balrl = hrrepl - hnrepl;
    if (balrl < -1 || balrl > 1) {
        return nrl;
    }

    return fix_height_nl(n_parent);
}

template <typename skey_t, typename sval_t, class RecMgr>
void bronson<skey_t, sval_t, RecMgr>::set_child(node_t<skey_t, sval_t> * volatile parent, node_t<skey_t, sval_t> * volatile child, bool_t is_right) {
    if (is_right) {
        parent->right = child;
    } else {
        parent->left = child;
    }
}

static volatile uint64_t meow;

template <typename skey_t, typename sval_t, class RecMgr>
sval_t bronson<skey_t, sval_t, RecMgr>::__contains(node_t<skey_t,sval_t>* curr, skey_t key, uint64_t ovl) {
    if (!curr) return NULL;
    skey_t ckey = curr->key;
    if (ckey == key) return curr->value;
#ifdef NO_OVL
    if (key < ckey) return __contains(curr->left, key, meow);
    return __contains(curr->right, key, meow);
#else
    if (key < ckey) return __contains(curr->left, key, curr->version);
    return __contains(curr->right, key, curr->version);
#endif
}

#endif /* BRONSON_IMPL_H */

