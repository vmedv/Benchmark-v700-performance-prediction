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
 * File:   howley_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 1, 2017, 12:52 PM
 */
//#pragma once
//#ifndef HOWLEY_IMPL_H
//#define HOWLEY_IMPL_H
#include "howley.h"

const unsigned int val_mask = ~(0x3); //TODO is that correct??

//Helper functions

static inline uint64_t GETFLAG(operation_t<skey_t,sval_t>* ptr) {
    return ((uint64_t)ptr) & 3;
}

static inline uint64_t FLAG(operation_t<skey_t,sval_t>* ptr, uint64_t flag) {
    return (((uint64_t)ptr) & 0xfffffffffffffffc) | flag;
}

static inline uint64_t UNFLAG(operation_t<skey_t,sval_t>* ptr) {
    return (((uint64_t)ptr) & 0xfffffffffffffffc);
}

//Last bit of the node pointer will be set to 1 if the pointer is null 
static inline uint64_t ISNULL(volatile node_t<skey_t,sval_t>* node){
	return (node == NULL) || (((uint64_t)node) & 1);
}

static inline uint64_t SETNULL(volatile node_t<skey_t,sval_t>* node){
	return (((uint64_t)node) & 0xfffffffffffffffe) | 1;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t,sval_t>* howley<skey_t, sval_t, RecMgr>::create_node(skey_t key, sval_t value, int initializing) {
    volatile node_t<skey_t,sval_t> * new_node;
    new_node = (volatile node_t<skey_t,sval_t>*) tree_malloc::allocnode(sizeof (node_t<skey_t,sval_t>));
    if (new_node == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }
    new_node->key = key;
    new_node->value = value;
    new_node->op = NULL;
    new_node->right = NULL;
    new_node->left = NULL;

    asm volatile("" :: : "memory");
    return (node_t<skey_t,sval_t>*) new_node;
}

template <typename skey_t, typename sval_t, class RecMgr>
operation_t<skey_t,sval_t>* howley<skey_t, sval_t, RecMgr>::alloc_op() {
    volatile operation_t<skey_t,sval_t> * new_op;
    new_op = (volatile operation_t<skey_t,sval_t>*) tree_malloc::alloc(sizeof (operation_t<skey_t,sval_t>));
    if (new_op == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }
    return (operation_t<skey_t,sval_t>*) new_op;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t howley<skey_t, sval_t, RecMgr>::bst_contains(skey_t k, node_t<skey_t,sval_t>* root) {

    node_t<skey_t,sval_t>* pred;
    node_t<skey_t,sval_t>* curr;
    operation_t<skey_t,sval_t>* pred_op;
    operation_t<skey_t,sval_t>* curr_op;
    intptr_t res = bst_find(k, &pred, &pred_op, &curr, &curr_op, root, root);
    if (res & val_mask) return (sval_t) res;
    return 0;
}

template <typename skey_t, typename sval_t, class RecMgr>
intptr_t howley<skey_t, sval_t, RecMgr>::bst_find(skey_t k, node_t<skey_t,sval_t>** pred, operation_t<skey_t,sval_t>** pred_op, node_t<skey_t,sval_t>** curr, operation_t<skey_t,sval_t>** curr_op, node_t<skey_t,sval_t>* aux_root, node_t<skey_t,sval_t>* root) {

    intptr_t result;
    skey_t curr_key;
    node_t<skey_t,sval_t>* next;
    node_t<skey_t,sval_t>* last_right;
    operation_t<skey_t,sval_t>* last_right_op;

retry:

    result = NOT_FOUND_R;
    *curr = aux_root;
    *curr_op = (*curr)->op;

    if (GETFLAG(*curr_op) != STATE_OP_NONE) {
        if (aux_root == root) {
            bst_help_child_cas((operation_t<skey_t,sval_t>*) UNFLAG(*curr_op), *curr, root);
            goto retry;
        } else {
            return ABORT;
        }
    }


    next = (node_t<skey_t,sval_t>*) (*curr)->right;
    last_right = *curr;
    last_right_op = *curr_op;

    while (!ISNULL(next)) {
        *pred = *curr;
        *pred_op = *curr_op;
        *curr = next;
        *curr_op = (*curr)->op;


        if (GETFLAG(*curr_op) != STATE_OP_NONE) {
            bst_help(*pred, *pred_op, *curr, *curr_op, root);
            goto retry;
        }
        curr_key = (*curr)->key;
        if (k < curr_key) {
            result = NOT_FOUND_L;
            next = (node_t<skey_t,sval_t>*) (*curr)->left;
        } else if (k > curr_key) {
            result = NOT_FOUND_R;
            next = (node_t<skey_t,sval_t>*) (*curr)->right;
            last_right = *curr;
            last_right_op = *curr_op;
        } else {
            result = (intptr_t) (*curr)->value;
            break;
        }
    }

    if ((!(result & val_mask)) && (last_right_op != last_right->op)) {
        goto retry;
    }

    if ((*curr)->op != *curr_op) {
        goto retry;
    }

    return result;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t howley<skey_t, sval_t, RecMgr>::bst_add(skey_t k, sval_t v, node_t<skey_t,sval_t>* root) {

    node_t<skey_t,sval_t>* pred;
    node_t<skey_t,sval_t>* curr;
    node_t<skey_t,sval_t>* new_node = NULL;
    operation_t<skey_t,sval_t>* pred_op;
    operation_t<skey_t,sval_t>* curr_op;
    operation_t<skey_t,sval_t>* cas_op;
    intptr_t result;

    while (TRUE) {
        result = bst_find(k, &pred, &pred_op, &curr, &curr_op, root, root);
        if (result & val_mask) {
#ifdef URCU_GC
            if (new_node != NULL) {
                free_node(new_node);
            }
#endif
            return (sval_t) result;
        }

        if (new_node == NULL) {
            new_node = create_node(k, v, 0);
        }

        bool_t is_left = (result == NOT_FOUND_L);
        node_t<skey_t,sval_t>* old;
        if (is_left) {
            old = (node_t<skey_t,sval_t>*) curr->left;
        } else {
            old = (node_t<skey_t,sval_t>*) curr->right;
        }

        cas_op = alloc_op();
        cas_op->child_cas_op.is_left = is_left;
        cas_op->child_cas_op.expected = old;
        cas_op->child_cas_op.update = new_node;


        if (BOOL_CAS(&curr->op, curr_op, FLAG(cas_op, STATE_OP_CHILDCAS))) {


            bst_help_child_cas(cas_op, curr, root);
#ifdef URCU_GC
            if (curr_op != NULL) free_node(curr_op);
#endif
            return NULL;
        } else {
#ifdef URCU_GC
            free_node(cas_op);
#endif
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void howley<skey_t, sval_t, RecMgr>::bst_help_child_cas(operation_t<skey_t,sval_t>* op, node_t<skey_t,sval_t>* dest, node_t<skey_t,sval_t>* root) {
    node_t<skey_t,sval_t>** address = NULL;
    if (op->child_cas_op.is_left) {
        address = (node_t<skey_t,sval_t>**) &(dest->left);
    } else {
        address = (node_t<skey_t,sval_t>**) &(dest->right);
    }

    BOOL_CAS(address, op->child_cas_op.expected, op->child_cas_op.update);
    BOOL_CAS(&(dest->op), FLAG(op, STATE_OP_CHILDCAS), FLAG(op, STATE_OP_NONE));

}

/* NOT WORKING WITH THE SVAL VS (intptr_t) stuff 
template <typename skey_t, typename sval_t, class RecMgr>
sval_t howley<skey_t, sval_t, RecMgr>::bst_remove(skey_t k, node_t<skey_t,sval_t>* root){

        node_t<skey_t,sval_t>* pred;
        node_t<skey_t,sval_t>* curr;
        node_t<skey_t,sval_t>* replace;
    sval_t val;
        operation_t<skey_t,sval_t>* pred_op;
        operation_t<skey_t,sval_t>* curr_op;
        operation_t<skey_t,sval_t>* replace_op;
        operation_t<skey_t,sval_t>* reloc_op=NULL;

        while(TRUE) {

        intptr_t res = bst_find(k, &pred, &pred_op, &curr, &curr_op, root, root);
                if (!(res & val_mask)) {
                        return NULL;
                }

                if (ISNULL((node_t<skey_t,sval_t>*) curr->right) || ISNULL((node_t<skey_t,sval_t>*) curr->left)) { // node has less than two children
    #ifdef FAKE_UPDATE
      if (BOOL_CAS(&(curr->op), curr_op, curr_op)) {
    #else
                        if (BOOL_CAS(&(curr->op), curr_op, FLAG(curr_op, STATE_OP_MARK))) {
    #endif
                                bst_help_marked(pred, pred_op, curr, root);
        #ifdef URCU_GC
        if (UNFLAG(curr->op)!=0) free_node((void*)UNFLAG(curr->op));
        free_node(curr);
        #endif
                                return (sval_t) res;
                        }
                } else { // node has two children
                        val = bst_find(k, &pred, &pred_op, &replace, &replace_op, curr, root);
                        if ((val == ABORT) || (curr->op != curr_op)) {
                                continue;
                        } 
            
            //if (reloc_op==NULL) {
                            reloc_op = alloc_op(); 
            //}
                        reloc_op->relocate_op.state = STATE_OP_ONGOING;
                        reloc_op->relocate_op.dest = curr;
                        reloc_op->relocate_op.dest_op = curr_op;
                        reloc_op->relocate_op.remove_key = k;
                        reloc_op->relocate_op.remove_value = res;
                        reloc_op->relocate_op.replace_key = replace->key;
                        reloc_op->relocate_op.replace_value = replace->value;

      #ifdef FAKE_UPDATE
      if (BOOL_CAS(&(replace->op), replace_op, replace_op)) {
      #else
                        if (BOOL_CAS(&(replace->op), replace_op, FLAG(reloc_op, STATE_OP_RELOCATE))) {
      #endif 
        #ifdef URCU_GC
          if (UNFLAG(replace_op)!=0) free_node((void*)UNFLAG(replace_op));
        #endif
                                if (bst_help_relocate(reloc_op, pred, pred_op, replace, root)) {
                   // if (UNFLAG(replace->op)!=0) ssmem_free(alloc,(void*)UNFLAG(replace->op));
                                        return res;
                                }
                        } else {
        #ifdef URCU_GC
          free_node(reloc_op);
        #endif
            }
                }
        }
}
 */

template <typename skey_t, typename sval_t, class RecMgr>
bool_t howley<skey_t, sval_t, RecMgr>::bst_help_relocate(operation_t<skey_t,sval_t>* op, node_t<skey_t,sval_t>* pred, operation_t<skey_t,sval_t>* pred_op, node_t<skey_t,sval_t>* curr, node_t<skey_t,sval_t>* root) {
    int seen_state = op->relocate_op.state;
    if (seen_state == STATE_OP_ONGOING) {
        // VCAS in original implementation
        operation_t<skey_t,sval_t>* seen_op = CAS_PTR(&(op->relocate_op.dest->op), op->relocate_op.dest_op, FLAG(op, STATE_OP_RELOCATE));
        if ((seen_op == op->relocate_op.dest_op) || (seen_op == (operation_t<skey_t,sval_t> *) FLAG(op, STATE_OP_RELOCATE))) {
            CAS_U32(&(op->relocate_op.state), STATE_OP_ONGOING, STATE_OP_SUCCESSFUL);
            seen_state = STATE_OP_SUCCESSFUL;
            if (seen_op == op->relocate_op.dest_op) {
#ifdef URCU_GC
                if (UNFLAG(seen_op) != 0) free_node((void*) UNFLAG(seen_op));
#endif
            }
        } else {
            // VCAS in original implementation
            seen_state = CAS_U32(&(op->relocate_op.state), STATE_OP_ONGOING, STATE_OP_FAILED);
        }
    }

    if (seen_state == STATE_OP_SUCCESSFUL) {

        BOOL_CAS(&(op->relocate_op.dest->key), op->relocate_op.remove_key, op->relocate_op.replace_key);
        BOOL_CAS(&(op->relocate_op.dest->value), op->relocate_op.remove_value, op->relocate_op.replace_value);
        BOOL_CAS(&(op->relocate_op.dest->op), FLAG(op, STATE_OP_RELOCATE), FLAG(op, STATE_OP_NONE));
    }

    bool_t result = (seen_state == STATE_OP_SUCCESSFUL);
    if (op->relocate_op.dest == curr) {
        return result;
    }

    BOOL_CAS(&(curr->op), FLAG(op, STATE_OP_RELOCATE), FLAG(op, result ? STATE_OP_MARK : STATE_OP_NONE));

    if (result) {
        if (op->relocate_op.dest == pred) {
            pred_op = (operation_t<skey_t,sval_t> *) FLAG(op, STATE_OP_NONE);
        }
        bst_help_marked(pred, pred_op, curr, root);
    }
#ifdef FAKE_UPDATE
    return TRUE;
#else
    return result;
#endif 
}

template <typename skey_t, typename sval_t, class RecMgr>
void howley<skey_t, sval_t, RecMgr>::bst_help_marked(node_t<skey_t,sval_t>* pred, operation_t<skey_t,sval_t>* pred_op, node_t<skey_t,sval_t>* curr, node_t<skey_t,sval_t>* root) {

    node_t<skey_t,sval_t>* new_ref;
    if (ISNULL((node_t<skey_t,sval_t>*) curr->left)) {
        if (ISNULL((node_t<skey_t,sval_t>*) curr->right)) {
            new_ref = (node_t<skey_t,sval_t>*) SETNULL(curr);
        } else {
            new_ref = (node_t<skey_t,sval_t>*) curr->right;
        }
    } else {
        new_ref = (node_t<skey_t,sval_t>*) curr->left;
    }
    operation_t<skey_t,sval_t>* cas_op = alloc_op();
    cas_op->child_cas_op.is_left = (curr == pred->left);
    cas_op->child_cas_op.expected = curr;
    cas_op->child_cas_op.update = new_ref;


    if (BOOL_CAS(&(pred->op), pred_op, FLAG(cas_op, STATE_OP_CHILDCAS))) {
        bst_help_child_cas(cas_op, pred, root);
#ifdef URCU_GC
        if (UNFLAG(pred_op) != 0) free_node((void*) UNFLAG(pred_op));
#endif
    } else {
#ifdef URCU_GC
        free_node(cas_op);
#endif
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void howley<skey_t, sval_t, RecMgr>::bst_help(node_t<skey_t,sval_t>* pred, operation_t<skey_t,sval_t>* pred_op, node_t<skey_t,sval_t>* curr, operation_t<skey_t,sval_t>* curr_op, node_t<skey_t,sval_t>* root) {
    if (GETFLAG(curr_op) == STATE_OP_CHILDCAS) {
        bst_help_child_cas((operation_t<skey_t,sval_t>*) UNFLAG(curr_op), curr, root);
    } else if (GETFLAG(curr_op) == STATE_OP_RELOCATE) {
        bst_help_relocate((operation_t<skey_t,sval_t>*) UNFLAG(curr_op), pred, pred_op, curr, root);
    } else if (GETFLAG(curr_op) == STATE_OP_MARK) {
        bst_help_marked(pred, pred_op, curr, root);
    }
}




//#endif /* HOWLEY_IMPL_H */

