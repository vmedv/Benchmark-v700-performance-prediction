/*   
 *   File: bst_tk.c
 *   Author: Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>
 *   Description: Asynchronized Concurrency: The Secret to Scaling Concurrent
 *    Search Data Structures, Tudor David, Rachid Guerraoui, Vasileios Trigonakis,
 *   ASPLOS '15
 *   bst_tk.c is part of ASCYLIB
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
 * File:   ticket_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 1:38 PM
 */

#ifndef TICKET_IMPL_H
#define TICKET_IMPL_H

#include "ticket.h"
#include <malloc.h>

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* ticket<skey_t, sval_t, RecMgr>::new_node(skey_t key, sval_t val, node_t<skey_t, sval_t>* l, node_t<skey_t, sval_t>* r, int initializing)
{
  node_t<skey_t, sval_t>* node;
  node = (node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof(node_t<skey_t, sval_t>));
  
  if (node == NULL) 
    {
      perror("malloc @ new_node");
      exit(1);
    }

  node->key = key;
  #ifndef NO_VAL
  node->val = val;
  #endif
  node->left = l;
  node->right = r;
  node->lock.to_uint64 = 0;

  return (node_t<skey_t, sval_t>*) node;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* ticket<skey_t, sval_t, RecMgr>::new_node_no_init()
{
  node_t<skey_t, sval_t>* node;

  node = (node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof(node_t<skey_t, sval_t>));

  if (unlikely(node == NULL))
    {
      perror("malloc @ new_node");
      exit(1);
    }
  #ifndef NO_VAL
  node->val = 0;
  #endif 
  node->lock.to_uint64 = 0;

  return (node_t<skey_t, sval_t>*) node;
}


template <typename skey_t, typename sval_t, class RecMgr>
intset_t<skey_t, sval_t>* ticket<skey_t, sval_t, RecMgr>::set_new()
{
  intset_t<skey_t, sval_t> *set;

  if ((set = (intset_t<skey_t, sval_t>*)memalign(CACHE_LINE_SIZE, sizeof(intset_t<skey_t, sval_t>))) == NULL) 
    {
      perror("malloc");
      exit(1);
    }
  node_t<skey_t, sval_t>* min = new_node(INT_MIN, SpecialNull, NULL, NULL, 1);
  node_t<skey_t, sval_t>* max = new_node(INT_MAX, SpecialNull, NULL, NULL, 1);
  set->head = new_node(INT_MAX, 0, min, max, 1);
  MEM_BARRIER;
  return set;
}

template <typename skey_t, typename sval_t, class RecMgr>
void ticket<skey_t, sval_t, RecMgr>::node_delete(node_t<skey_t, sval_t> *node) 
{

}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t ticket<skey_t, sval_t, RecMgr>::bst_tk_delete(intset_t<skey_t, sval_t>* set, skey_t key)
{
  node_t<skey_t, sval_t>* curr;
  node_t<skey_t, sval_t>* pred = NULL;
  node_t<skey_t, sval_t>* ppred = NULL;
  volatile uint64_t curr_ver = 0;
  uint64_t pred_ver = 0, ppred_ver = 0, right = 0, pright = 0;

 retry:

  curr = set->head;

  do
    {
      curr_ver = curr->lock.to_uint64;

      ppred = pred;
      ppred_ver = pred_ver;
      pright = right;

      pred = curr;
      pred_ver = curr_ver;

      if (key < curr->key)
	{
	  right = 0;
	  curr = (node_t<skey_t, sval_t>*) curr->left;
	}
      else
	{
	  right = 1;
	  curr = (node_t<skey_t, sval_t>*) curr->right;
	}
    }
  #ifdef NO_VAL
  while(!(curr->right == NULL && curr->left == NULL));
  #else
  while(likely(!curr->leaf));
  #endif


  if (curr->key != key)
    {
      return 0;
    }

  if ((!tl_trylock_version(&ppred->lock, (volatile tl_t*) &ppred_ver, pright)))
    {
      goto retry;
    }

  if ((!tl_trylock_version_both(&pred->lock, (volatile tl_t*) &pred_ver)))
    {
      tl_revert(&ppred->lock, pright);
      goto retry;
    }

  if (pright)
    {
      if (right)
	{
	  ppred->right = pred->left;
	}
      else
	{
	  ppred->right = pred->right;
	}
    }
  else
    {
      if (right)
	{
	  ppred->left = pred->left;
	}
      else
	{
	  ppred->left = pred->right;
	}
    }

  tl_unlock(&ppred->lock, pright);
  
  #if URCU_GC
    free_node(curr);
    free_node(pred);
 #endif
  
  #ifdef NO_VAL
  return 1; 
  #else
  return curr->val;
  #endif
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t ticket<skey_t, sval_t, RecMgr>::bst_tk_find(intset_t<skey_t, sval_t>* set, skey_t key) 
{
  node_t<skey_t, sval_t>* curr = set->head;
  
  #ifdef NO_VAL
  while(!(curr->right == NULL && curr->left == NULL))
  #else
  while (likely(!curr->leaf))
  #endif
    {
      if (key < curr->key)
	{
	  curr = (node_t<skey_t, sval_t>*) curr->left;
	}
      else
	{
	  curr = (node_t<skey_t, sval_t>*) curr->right;
	}
    }

  if (curr->key == key)
    {
      #ifdef NO_VAL
      return 1;
      #else
      return curr->val;
      #endif
    }  

  return 0;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t ticket<skey_t, sval_t, RecMgr>::bst_tk_insert(intset_t<skey_t, sval_t>* set, skey_t key, sval_t val) 
{
  node_t<skey_t, sval_t>* curr;
  node_t<skey_t, sval_t>* pred = NULL;
  volatile uint64_t curr_ver = 0;
  uint64_t pred_ver = 0, right = 0;

 retry:

  curr = set->head;

  do
    {
      curr_ver = curr->lock.to_uint64;

      pred = curr;
      pred_ver = curr_ver;

      if (key < curr->key)
	{
	  right = 0;
	  curr = (node_t<skey_t, sval_t>*) curr->left;
	}
      else
	{
	  right = 1;
	  curr = (node_t<skey_t, sval_t>*) curr->right;
	}
    }
  #ifdef NO_VAL
  while (!(curr->right == NULL && curr->left == NULL));
  #else
  while (likely(!curr->leaf));
  #endif


  if (curr->key == key)
    {
      return curr->val;
    }

  node_t<skey_t, sval_t>* nn = new_node(key, val, NULL, NULL, 0);
  node_t<skey_t, sval_t>* nr = new_node_no_init();

  if ((!tl_trylock_version(&pred->lock, (volatile tl_t*) &pred_ver, right)))
    {
      tree_malloc::free(nn);
      tree_malloc::free(nr);
      goto retry;
    }

  if (key < curr->key)
    {
      nr->key = curr->key;
      nr->left = nn;
      nr->right = curr;
    }
  else
    {
      nr->key = key;
      nr->left = curr;
      nr->right = nn;
    }

  if (right)
    {
      pred->right = nr;
    }
  else
    {
      pred->left = nr;
    }


  tl_unlock(&pred->lock, right);

  return NULL;
}


#endif /* TICKET_IMPL_H */

