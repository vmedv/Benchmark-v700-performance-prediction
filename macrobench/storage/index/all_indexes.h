/* 
 * File:   all_indexes.h
 * Author: trbot
 *
 * Created on May 28, 2017, 4:33 PM
 */

#ifndef ALL_INDEXES_H
#define ALL_INDEXES_H

#include "config.h"

#if 0 // nothing (convenience #if to make ALL below use #elif)
#elif (INDEX_STRUCT == IDX_BST_RQ_LOCKFREE) || \
      (INDEX_STRUCT == IDX_BST_RQ_RWLOCK) || \
      (INDEX_STRUCT == IDX_BST_RQ_HTM_RWLOCK) || \
      (INDEX_STRUCT == IDX_BST_RQ_UNSAFE) || \
      (INDEX_STRUCT == IDX_CITRUS_RQ_LOCKFREE) || \
      (INDEX_STRUCT == IDX_CITRUS_RQ_RWLOCK) || \
      (INDEX_STRUCT == IDX_CITRUS_RQ_HTM_RWLOCK) || \
      (INDEX_STRUCT == IDX_CITRUS_RQ_UNSAFE) || \
      (INDEX_STRUCT == IDX_RLUCITRUS) || \
      (INDEX_STRUCT == IDX_ABTREE_RQ_LOCKFREE) || \
      (INDEX_STRUCT == IDX_ABTREE_RQ_RWLOCK) || \
      (INDEX_STRUCT == IDX_ABTREE_RQ_HTM_RWLOCK) || \
      (INDEX_STRUCT == IDX_ABTREE_RQ_UNSAFE) || \
      (INDEX_STRUCT == IDX_SKIPLISTLOCK_RQ_LOCKFREE) || \
      (INDEX_STRUCT == IDX_SKIPLISTLOCK_RQ_RWLOCK) || \
      (INDEX_STRUCT == IDX_SKIPLISTLOCK_RQ_HTM_RWLOCK) || \
      (INDEX_STRUCT == IDX_SKIPLISTLOCK_RQ_UNSAFE) || \
      (INDEX_STRUCT == IDX_SKIPLISTLOCK_RQ_SNAPCOLLECTOR)
#include "index_with_rq.h"
#elif (INDEX_STRUCT == IDX_BTREE)
#include "index_btree.h"
#elif (INDEX_STRUCT == IDX_BST)
#include "index_bst.h"
#elif (INDEX_STRUCT == IDX_ABTREE)
#include "index_abtree.h"
#elif   (INDEX_STRUCT == IDX_HOWLEY) || \
        (INDEX_STRUCT == IDX_HOWLEY_PAD) || \
        (INDEX_STRUCT == IDX_HOWLEY_PAD_LARGE_DES) || \
        (INDEX_STRUCT == IDX_HOWLEY_BASELINE) || \
        (INDEX_STRUCT == IDX_ELLEN) || \
        (INDEX_STRUCT == IDX_ELLEN_PAD) || \
        (INDEX_STRUCT == IDX_ELLEN_BASELINE) ||\
        (INDEX_STRUCT == IDX_WFRBT) || \
        (INDEX_STRUCT == IDX_WFRBT_BASELINE) || \
        (INDEX_STRUCT == IDX_WFRBT_ASCY) || \
        (INDEX_STRUCT == IDX_WFRBT_ASCY_BASELINE) || \
        (INDEX_STRUCT == IDX_BRONSON_SPIN) || \
        (INDEX_STRUCT == IDX_BRONSON_SPIN_NO_REREAD) || \
        (INDEX_STRUCT == IDX_BRONSON_SPIN_NO_OVL) || \
        (INDEX_STRUCT == IDX_BRONSON_BASELINE) || \
        (INDEX_STRUCT == IDX_CCAVL_SPIN) || \
        (INDEX_STRUCT == IDX_CCAVL_SPIN_NO_REREAD) || \
        (INDEX_STRUCT == IDX_CCAVL_SPIN_NO_OVL) || \
        (INDEX_STRUCT == IDX_CCAVL_BASELINE) || \
        (INDEX_STRUCT == IDX_DANA_SPIN_FIELDS) || \
        (INDEX_STRUCT == IDX_DANA_SPIN_PAD_FIELDS) || \
        (INDEX_STRUCT == IDX_DANA_SPIN_FIELDS_3_LINES) || \
        (INDEX_STRUCT == IDX_DANA_BASELINE) || \
        (INDEX_STRUCT == IDX_CITRUS_SPIN) || \
        (INDEX_STRUCT == IDX_CITRUS_SPIN_PAD) || \
        (INDEX_STRUCT == IDX_CITRUS_BASELINE) || \
        (INDEX_STRUCT == IDX_BONSAI) || \
        (INDEX_STRUCT == IDX_BONSAI_PAD) || \
        (INDEX_STRUCT == IDX_BONSAI_BASELINE) || \
        (INDEX_STRUCT == IDX_INTLF) || \
        (INDEX_STRUCT == IDX_INTLF_PAD) || \
        (INDEX_STRUCT == IDX_INTLF_BASELINE) || \
        (INDEX_STRUCT == IDX_TICKET) || \
        (INDEX_STRUCT == IDX_TICKET_PAD) || \
        (INDEX_STRUCT == IDX_TICKET_BASELINE) 
#include "index_anomaly_bst.h"
#elif (INDEX_STRUCT == IDX_HASH)
#include "index_hash.h"
#else
#error Must define INDEX_STRUCT to be one of the options in storage/index/all_indexes.h
#endif

#endif /* ALL_INDEXES_H */

