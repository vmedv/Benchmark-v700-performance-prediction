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
 * File:   wfrbt_ascy_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 10, 2017, 9:41 AM
 */

#ifndef WFRBT_ASCY_IMPL_H
#define WFRBT_ASCY_IMPL_H

#include "wfrbt_ascy.h"

static inline void set_bit(volatile uintptr_t* *array, int bit) {
    asm("bts %1,%0" : "+m" (*array) : "r" (bit));
}

static inline bool_t set_bit2(volatile uintptr_t *array, int bit) {

    // asm("bts %1,%0" :  "+m" (*array): "r" (bit));
    bool_t flag;
    __asm__ __volatile__("lock bts %2,%1; setb %0" : "=q" (flag) : "m" (*array), "r" (bit));
    return flag;
    return flag;
}

static inline uint64_t GETFLAG(volatile node_t<skey_t, sval_t>* ptr) {
    return ((uint64_t)ptr) & 1;
}

static inline uint64_t GETTAG(volatile node_t<skey_t, sval_t>* ptr) {
    return ((uint64_t)ptr) & 2;
}

static inline uint64_t FLAG(node_t<skey_t, sval_t>* ptr) {
    return (((uint64_t)ptr)) | 1;
}

static inline uint64_t TAG(node_t<skey_t, sval_t>* ptr) {
    return (((uint64_t)ptr)) | 2;
}

static inline uint64_t UNTAG(node_t<skey_t, sval_t>* ptr) {
    return (((uint64_t)ptr) & 0xfffffffffffffffd);
}

static inline uint64_t UNFLAG(node_t<skey_t, sval_t>* ptr) {
    return (((uint64_t)ptr) & 0xfffffffffffffffe);
}

static inline node_t<skey_t, sval_t>* ADDRESS(volatile node_t<skey_t, sval_t>* ptr) {
    return (node_t<skey_t, sval_t>*) (((uint64_t)ptr) & 0xfffffffffffffffc);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* wfrbt_ascy<skey_t, sval_t, RecMgr>::initialize_tree(){
    node_t<skey_t, sval_t>* r;
    node_t<skey_t, sval_t>* s;
    node_t<skey_t, sval_t>* inf0;
    node_t<skey_t, sval_t>* inf1;
    node_t<skey_t, sval_t>* inf2;
    r = create_node(INF2,NULL,1);
    s = create_node(INF1,NULL,1);
    inf0 = create_node(INF0,NULL,1);
    inf1 = create_node(INF1,NULL,1);
    inf2 = create_node(INF2,NULL,1);
    
    asm volatile("" ::: "memory");
    r->left = s;
    r->right = inf2;
    s->right = inf1;
    s->left= inf0;
    asm volatile("" ::: "memory");

    return r;
}



template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* wfrbt_ascy<skey_t, sval_t, RecMgr>::create_node(skey_t k, sval_t value, int initializing) {
    volatile node_t<skey_t, sval_t>* new_node;
    new_node = (volatile node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof(node_t<skey_t, sval_t>));
    if (new_node == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->key = k;
    new_node->value = value;
    asm volatile("" ::: "memory");
    return (node_t<skey_t, sval_t>*) new_node;
}

template <typename skey_t, typename sval_t, class RecMgr>
void wfrbt_ascy<skey_t, sval_t, RecMgr>::bst_seek(skey_t key, node_t<skey_t, sval_t>* node_r, seek_record_t<skey_t, sval_t>* seek_record){
//  PARSE_TRY();
    volatile seek_record_t<skey_t, sval_t> seek_record_l;
    node_t<skey_t, sval_t>* node_s = ADDRESS(node_r->left);
    seek_record_l.ancestor = node_r;
    seek_record_l.successor = node_s; 
    seek_record_l.parent = node_s;
    seek_record_l.leaf = ADDRESS(node_s->left);

    node_t<skey_t, sval_t>* parent_field = (node_t<skey_t, sval_t>*) seek_record_l.parent->left;
    node_t<skey_t, sval_t>* current_field = (node_t<skey_t, sval_t>*) seek_record_l.leaf->left;
    node_t<skey_t, sval_t>* current = ADDRESS(current_field);


    while (current != NULL) {
        if (!GETTAG(parent_field)) {
            seek_record_l.ancestor = seek_record_l.parent;
            seek_record_l.successor = seek_record_l.leaf;
        }
        seek_record_l.parent = seek_record_l.leaf;
        seek_record_l.leaf = current;

        parent_field = current_field;
        if (key < current->key) {
            current_field= (node_t<skey_t, sval_t>*) current->left;
        } else {
            current_field= (node_t<skey_t, sval_t>*) current->right;
        }
        current=ADDRESS(current_field);
    }
    seek_record->ancestor=seek_record_l.ancestor;
    seek_record->successor=seek_record_l.successor;
    seek_record->parent=seek_record_l.parent;
    seek_record->leaf=seek_record_l.leaf;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t wfrbt_ascy<skey_t, sval_t, RecMgr>::bst_search(skey_t key, node_t<skey_t, sval_t>* node_r) {
   #ifndef SEARCH
   seek_record_t<skey_t, sval_t> seek_record_struct; 
   seek_record_t<skey_t, sval_t>* seek_record = &seek_record_struct;
   bst_seek(key, node_r, seek_record);
   if (seek_record->leaf->key == key) {
        return seek_record->leaf->value;
   } else {
        return NULL;
   }
   #else
     node_t<skey_t, sval_t>* node_s = ADDRESS(node_r->left);
     //node_t<skey_t, sval_t>* parent_field = (node_t<skey_t, sval_t>*) node_s->left;
     node_t<skey_t, sval_t>* current_field = ADDRESS(node_s->left)->left;
     node_t<skey_t, sval_t>* current = ADDRESS(current_field);
     node_t<skey_t, sval_t>* leaf = ADDRESS(node_s->left);; 

      while (current != NULL) {
          leaf = current;
          if (key < current->key) {
              current_field= (node_t<skey_t, sval_t>*) current->left;
          } else {
              current_field= (node_t<skey_t, sval_t>*) current->right;
          }
          current=ADDRESS(current_field);
      }
      if (leaf->key != key){
         return NULL;
      } else {
          return leaf->value;
      }
   #endif
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t wfrbt_ascy<skey_t, sval_t, RecMgr>::bst_insert(skey_t key, sval_t val, node_t<skey_t, sval_t>* node_r) {
    node_t<skey_t, sval_t>* new_internal = NULL;
    node_t<skey_t, sval_t>* new_node = NULL;
    uint created = 0;
    
    seek_record_t<skey_t, sval_t> seek_record_struct; 
    seek_record_t<skey_t, sval_t>* seek_record = &seek_record_struct;
    
    while (1) {
//      UPDATE_TRY();
        bst_seek(key, node_r, seek_record);
        if (seek_record->leaf->key == key) {
#ifdef URCU_GC
            if (created) {
                free_node(new_internal);
                free_node(new_node);
            }
#endif
            return seek_record->leaf->value;
        }
        node_t<skey_t, sval_t>* parent = seek_record->parent;
        node_t<skey_t, sval_t>* leaf = seek_record->leaf;

        node_t<skey_t, sval_t>** child_addr;
        if (key < parent->key) {
            child_addr = (node_t<skey_t, sval_t>**) &(parent->left); 
        } else {
            child_addr= (node_t<skey_t, sval_t>**) &(parent->right);
        }
        if (likely(created==0)) {
            new_internal=create_node(max(key,leaf->key),NULL,0);
            new_node = create_node(key,val,0);
            created=1;
        } else {
            new_internal->key=max(key,leaf->key);
        }
        if ( key < leaf->key) {
            new_internal->left = new_node;
            new_internal->right = leaf; 
        } else {
            new_internal->right = new_node;
            new_internal->left = leaf;
        }
 #ifdef __tile__
    MEM_BARRIER;
#endif
        #ifdef FAKE_UPDATE
        node_t<skey_t, sval_t>* result = CAS_PTR(child_addr, ADDRESS(leaf), ADDRESS(leaf));
        #else
        node_t<skey_t, sval_t>* result = CAS_PTR(child_addr, ADDRESS(leaf), ADDRESS(new_internal));
        #endif
        if (result == ADDRESS(leaf)) {
            return NULL;
        }
        node_t<skey_t, sval_t>* chld = *child_addr; 
        if ( (ADDRESS(chld)==leaf) && (GETFLAG(chld) || GETTAG(chld)) ) {
            bst_cleanup(key,seek_record); 
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t wfrbt_ascy<skey_t, sval_t, RecMgr>::bst_remove(skey_t key, node_t<skey_t, sval_t>* node_r) {
    bool_t injecting = TRUE; 
    node_t<skey_t, sval_t>* leaf;
    sval_t val = 0;
    
    seek_record_t<skey_t, sval_t> seek_record_struct; 
    seek_record_t<skey_t, sval_t>* seek_record = &seek_record_struct;
    
    while (1) {
//      UPDATE_TRY();

        bst_seek(key, node_r,seek_record);
        val = seek_record->leaf->value;
        node_t<skey_t, sval_t>* parent = seek_record->parent;

        node_t<skey_t, sval_t>** child_addr;
        if (key < parent->key) {
            child_addr = (node_t<skey_t, sval_t>**) &(parent->left);
        } else {
            child_addr = (node_t<skey_t, sval_t>**) &(parent->right);
        }

        if (injecting == TRUE) {
            leaf = seek_record->leaf;
            if (leaf->key != key) {
                return 0;
            }
            node_t<skey_t, sval_t>* lf = ADDRESS(leaf);
            #ifdef FAKE_UPDATE
              node_t<skey_t, sval_t>* result = CAS_PTR(child_addr, lf, lf);
            #else
              node_t<skey_t, sval_t>* result = CAS_PTR(child_addr, lf, FLAG(lf));
            #endif 
            if (result == ADDRESS(leaf)) {
                injecting = FALSE;
                bool_t done = bst_cleanup(key, seek_record);
                if (done == TRUE) {
                    return val;
                }
            } else {
                node_t<skey_t, sval_t>* chld = *child_addr;
                if ( (ADDRESS(chld) == leaf) && (GETFLAG(chld) || GETTAG(chld)) ) {
                    bst_cleanup(key, seek_record);
                }
            }
        } else {
            if (seek_record->leaf != leaf) {
                return val; 
            } else {
                bool_t done = bst_cleanup(key,seek_record);
                if (done == TRUE) {
                    return val;
                }
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool_t wfrbt_ascy<skey_t, sval_t, RecMgr>::bst_cleanup(skey_t key, seek_record_t<skey_t, sval_t>* seek_record) {
    node_t<skey_t, sval_t>* ancestor = seek_record->ancestor;
    node_t<skey_t, sval_t>* successor = seek_record->successor;
    node_t<skey_t, sval_t>* parent = seek_record->parent;
    //node_t<skey_t, sval_t>* leaf = seek_record->leaf;

    node_t<skey_t, sval_t>** succ_addr;
    if (key < ancestor->key) {
        succ_addr = (node_t<skey_t, sval_t>**) &(ancestor->left);
    } else {
        succ_addr = (node_t<skey_t, sval_t>**) &(ancestor->right);
    }

    node_t<skey_t, sval_t>** child_addr;
    node_t<skey_t, sval_t>** sibling_addr;
    if (key < parent->key) {
       child_addr = (node_t<skey_t, sval_t>**) &(parent->left);
       sibling_addr = (node_t<skey_t, sval_t>**) &(parent->right);
    } else {
       child_addr = (node_t<skey_t, sval_t>**) &(parent->right);
       sibling_addr = (node_t<skey_t, sval_t>**) &(parent->left);
    }

    node_t<skey_t, sval_t>* chld = *(child_addr);
    if (!GETFLAG(chld)) {
        chld = *(sibling_addr);
        asm volatile("");
        sibling_addr = child_addr;
    }
//#if defined(__tile__) || defined(__sparc__)
    while (1) {
        node_t<skey_t, sval_t>* untagged = *sibling_addr;
        node_t<skey_t, sval_t>* tagged = (node_t<skey_t, sval_t>*)TAG(untagged);
        #ifdef FAKE_UPDATE
        node_t<skey_t, sval_t>* res = CAS_PTR(sibling_addr,untagged, untagged);
        #else
        node_t<skey_t, sval_t>* res = CAS_PTR(sibling_addr,untagged, tagged);
        #endif 
        if (res == untagged) {
            break;
         }
    }
//#else
//    set_bit(sibling_addr,1);
//#endif

    node_t<skey_t, sval_t>* sibl = *sibling_addr;
    #ifdef FAKE_UPDATE
     if ( CAS_PTR(succ_addr, ADDRESS(successor), ADDRESS(successor)) == ADDRESS(successor)) {
    #else
     if ( CAS_PTR(succ_addr, ADDRESS(successor), UNTAG(sibl)) == ADDRESS(successor)) {
    #endif 
#ifdef URCU_GC
    free_node(ADDRESS(chld));
    free_node(ADDRESS(successor));
#endif
        return TRUE;
    }
    return FALSE;
}



#endif /* WFRBT_ASCY_IMPL_H */

