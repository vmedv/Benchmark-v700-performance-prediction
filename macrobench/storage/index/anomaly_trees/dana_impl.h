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
 * File:   dana_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 3:25 PM
 */

#ifndef DANA_IMPL_H
#define DANA_IMPL_H

#include "dana.h"

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* dana<skey_t, sval_t, RecMgr>::create_node(skey_t k, sval_t value, int initializing) {
    volatile node_t<skey_t, sval_t>* new_node;

    new_node = (volatile node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof(node_t<skey_t, sval_t>));

    if (new_node == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->succ = NULL;
    new_node->pred = NULL;
    INIT_LOCK(&(new_node->tree_lock));
    INIT_LOCK(&(new_node->succ_lock));
    new_node->key = k;
    new_node->value = value;
    new_node->mark = FALSE;
    asm volatile("" ::: "memory");
    return (node_t<skey_t, sval_t>*) new_node;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* dana<skey_t, sval_t, RecMgr>::initialize_tree(){
    node_t<skey_t, sval_t>* parent = create_node(MIN_KEY, (sval_t) NULL, 1); 
    node_t<skey_t, sval_t>* root = create_node(MAX_KEY, (sval_t) NULL, 1);
    root->pred = parent;
    root->succ = parent;
    root->parent = parent;
    parent->right = root;
    parent->succ = root;
    return root;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* dana<skey_t, sval_t, RecMgr>::bst_search(skey_t k, node_t<skey_t, sval_t>* root) {
  //PARSE_TRY();

  node_t<skey_t, sval_t>* n = root;
  node_t<skey_t, sval_t>* child;
  skey_t curr_key;
  while (1) {
    curr_key = n->key;
    if (curr_key == k) {
      return n;
    }
    if ( curr_key < k ) {
      child = (node_t<skey_t, sval_t>*) n->right;
    } else {
      child = (node_t<skey_t, sval_t>*) n->left;
    }
    if ( child == NULL ) {
      return n;
    }
    n = child;
  }
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t dana<skey_t, sval_t, RecMgr>::bst_contains(skey_t k, node_t<skey_t, sval_t>* root) {
    node_t<skey_t, sval_t>* n = bst_search(k,root);
    while (n->key > k){
        n = (node_t<skey_t, sval_t>*) n->pred;
    }
    while (n->key < k){
        n = (node_t<skey_t, sval_t>*) n->succ;
    }
    if ((n->key == k) && (n->mark == FALSE)) {
        return n->value;
    }
    return NULL;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t dana<skey_t, sval_t, RecMgr>::bst_insert(skey_t k, sval_t v, node_t<skey_t, sval_t>* root) {
    while(1) {
      //UPDATE_TRY();
        node_t<skey_t, sval_t>* node = bst_search(k, root);
#if defined(NO_VOLATILE) || defined(BASELINE)
        volatile node_t<skey_t, sval_t>* p;
#else
        node_t<skey_t, sval_t>* volatile p;
#endif
        if (node->key >= k) {
            p = (node_t<skey_t, sval_t>*) node->pred;
        } else {
            p = (node_t<skey_t, sval_t>*) node;
        }

#if DRACHSLER_RO_FAIL == 1
	 node_t<skey_t, sval_t>* n = node;
	while (n->key > k)
	  {
	    n = (node_t<skey_t, sval_t>*) n->pred;
	  }
	while (n->key < k)
	  {
	    n = (node_t<skey_t, sval_t>*) n->succ;
	  }
	if ((n->key == k) && (n->mark == FALSE)) 
	  {
	    return NULL;
	  }
#endif

        LOCK(&(p->succ_lock));
#if defined(NO_VOLATILE) || defined(BASELINE)
        volatile node_t<skey_t, sval_t>* s = (node_t<skey_t, sval_t>*) p->succ;
#else
        node_t<skey_t, sval_t>* volatile s = (node_t<skey_t, sval_t>*) p->succ;
#endif
        if ((k > p->key) && (k <= s->key) && (p->mark == FALSE)) {
            if (s->key == k) {
                sval_t res = s->value;
                UNLOCK(&(p->succ_lock)); 
                return res;
            }
            node_t<skey_t, sval_t>* new_node = create_node(k,v,0);
            node_t<skey_t, sval_t>* parent = choose_parent((node_t<skey_t, sval_t>*) p, (node_t<skey_t, sval_t>*) s, node);
            new_node->succ = s;
            new_node->pred = p;
            new_node->parent = parent;
#ifdef __tile__
            MEM_BARRIER;
#endif
            s->pred = new_node;
            p->succ = new_node;
            UNLOCK(&(p->succ_lock));
            insert_to_tree((node_t<skey_t, sval_t>*) parent,(node_t<skey_t, sval_t>*) new_node,(node_t<skey_t, sval_t>*) root);
            return NULL;
        }
        UNLOCK(&(p->succ_lock));
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* dana<skey_t, sval_t, RecMgr>::choose_parent(node_t<skey_t, sval_t>* p, node_t<skey_t, sval_t>* s, node_t<skey_t, sval_t>* first_cand){
    node_t<skey_t, sval_t>* candidate;
    if ((first_cand == p) || (first_cand == s)) {
        candidate = first_cand;
    } else {
        candidate = p;
    }
    while (1) {
        LOCK(&(candidate->tree_lock));
        if (candidate == p) {
            if (candidate->right == NULL) {
                return candidate;
            }
            UNLOCK(&(candidate->tree_lock));
            candidate = s;
        } else {
            if (candidate->left == NULL) {
                return candidate;
            }
            UNLOCK(&(candidate->tree_lock));
            candidate = p;
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void dana<skey_t, sval_t, RecMgr>::insert_to_tree(node_t<skey_t, sval_t>* parent, node_t<skey_t, sval_t>* new_node, node_t<skey_t, sval_t>* root) {
    new_node->parent = parent;
    if (parent->key < new_node->key) {
        parent->right = new_node;
    } else {
        parent->left = new_node;
    }

    UNLOCK(&(parent->tree_lock));
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* dana<skey_t, sval_t, RecMgr>::lock_parent(node_t<skey_t, sval_t>* node) {
    node_t<skey_t, sval_t>* p;
    while (1) {
        p = (node_t<skey_t, sval_t>*) node->parent;
        LOCK(&(p->tree_lock));
        if ((node->parent == p) && (p->mark == FALSE)) {
            return p;
        }
        UNLOCK(&(p->tree_lock));
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t dana<skey_t, sval_t, RecMgr>::bst_remove(skey_t k, node_t<skey_t, sval_t>* root) {
    node_t<skey_t, sval_t>* node;
    while (1) {
      //UPDATE_TRY();
        node = bst_search(k, root);
        node_t<skey_t, sval_t>* p;
        if (node->key >= k) {
            p = (node_t<skey_t, sval_t>*) node->pred;
        } else {
            p = (node_t<skey_t, sval_t>*) node;
        }

#if DRACHSLER_RO_FAIL == 1
	node_t<skey_t, sval_t>* n = node;
	while (n->key > k)
	  {
	    n = (node_t<skey_t, sval_t>*) n->pred;
	  }
	while (n->key < k)
	  {
	    n = (node_t<skey_t, sval_t>*) n->succ;
	  }
	if ((n->key != k) && (n->mark == FALSE)) 
	  {
	    return FALSE;
	  }
#endif

        LOCK(&(p->succ_lock));
        node_t<skey_t, sval_t>* s = (node_t<skey_t, sval_t>*) p->succ;
        if ((k > p->key) && (k <= s->key) && (p->mark == FALSE)) {
            if (s->key > k) {
                UNLOCK(&(p->succ_lock));
                return 0;
            }
            LOCK(&(s->succ_lock));
            bool_t has_two_children = acquire_tree_locks(s);
            lock_parent(s);
            s->mark = TRUE;
            node_t<skey_t, sval_t>* s_succ = (node_t<skey_t, sval_t>*) s->succ;
            s_succ->pred = p;
            p->succ = s_succ;
            UNLOCK(&(s->succ_lock));
            UNLOCK(&(p->succ_lock));
            sval_t v = s->value;
            remove_from_tree(s, has_two_children,root);
            return v; 
        }
        UNLOCK(&(p->succ_lock));
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool_t dana<skey_t, sval_t, RecMgr>::acquire_tree_locks(node_t<skey_t, sval_t>* n) {
  //LOCK_TRY_ONCE_CLEAR();

    while (1) {
        LOCK(&(n->tree_lock));
        node_t<skey_t, sval_t>* left = (node_t<skey_t, sval_t>*) n->left;
        node_t<skey_t, sval_t>* right = (node_t<skey_t, sval_t>*) n->right;
        //lock_parent(n);
        if ((right == NULL) || (left == NULL)) {
            return FALSE;
        } else {
            node_t<skey_t, sval_t>* s = (node_t<skey_t, sval_t>*) n->succ;
            int l=0;
            node_t<skey_t, sval_t>* parent;
            node_t<skey_t, sval_t>* sp = (node_t<skey_t, sval_t>*) s->parent;
            if (sp != n) {
                parent = sp;
                //TRYLOCK failure returns non-zero value!
                //if (trylock failure) {...} 
                if (TRYLOCK(&(parent->tree_lock))!=0) {
                    UNLOCK(&(n->tree_lock));
                    //UNLOCK(&(n->parent->tree_lock));
                    continue;
                }
                l=1;
                if ((parent != s->parent) || (parent->mark==TRUE)) {
                    UNLOCK(&(n->tree_lock));
                    UNLOCK(&(parent->tree_lock));
                    //UNLOCK(&(n->parent->tree_lock));
                    continue;
                }
            }
            //TRYLOCK failure returns non-zero value!
            //if (trylock failure) {...} 
            if (TRYLOCK(&(s->tree_lock))!=0) {
                UNLOCK(&(n->tree_lock));
                //UNLOCK(&(n->parent->tree_lock));
                if (l) { 
                    UNLOCK(&(parent->tree_lock));
                }
                continue;
            }
            return TRUE;
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void dana<skey_t, sval_t, RecMgr>::remove_from_tree(node_t<skey_t, sval_t>* n, bool_t has_two_children,node_t<skey_t, sval_t>* root) {
    node_t<skey_t, sval_t>* child;
    node_t<skey_t, sval_t>* parent;
    node_t<skey_t, sval_t>* s;
    //int l=0;
    if (has_two_children == FALSE) { 
        if ( n->right == NULL) {
            child = (node_t<skey_t, sval_t>*) n->left;
        } else {
            child = (node_t<skey_t, sval_t>*) n->right;
        }
        parent = (node_t<skey_t, sval_t>*) n->parent;
        update_child(parent, n, child);
    } else {
        s = (node_t<skey_t, sval_t>*) n->succ;
        child = (node_t<skey_t, sval_t>*) s->right;
        parent = (node_t<skey_t, sval_t>*) s->parent;
        //if (parent != n ) l=1;
        update_child(parent, s, child);
        s->left = n->left;
        s->right = n->right;
        n->left->parent = s;
        if (n->right != NULL) {
            n->right->parent = s;
        }
        update_child((node_t<skey_t, sval_t>*) n->parent, n, s);
        if (parent == n) {
            parent = s;
        } else {
            UNLOCK(&(s->tree_lock));
        }
        UNLOCK(&(parent->tree_lock));
    }
    UNLOCK(&(n->parent->tree_lock));
    UNLOCK(&(n->tree_lock));

#ifdef URCU_GC
    free_node(n);
#endif
}

template <typename skey_t, typename sval_t, class RecMgr>
void dana<skey_t, sval_t, RecMgr>::update_child(node_t<skey_t, sval_t>* parent, node_t<skey_t, sval_t>* old_ch, node_t<skey_t, sval_t>* new_ch) {
    if (parent->left == old_ch) {
        parent->left = new_ch;
    } else {
        parent->right = new_ch;
    }
    if (new_ch != NULL) {
        new_ch->parent = parent;
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
uint32_t dana<skey_t, sval_t, RecMgr>::bst_size(node_t<skey_t, sval_t>* node) {
    if (node==NULL) return 0;
    uint32_t x = 0;
    if ((node->key != MAX_KEY) && (node->key != MIN_KEY)) {
        x = 1;
    }
    return x + bst_size((node_t<skey_t, sval_t>*) node->right) + bst_size((node_t<skey_t, sval_t>*) node->left);
}

#endif /* DANA_IMPL_H */

