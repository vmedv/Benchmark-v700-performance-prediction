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
 * File:   ticket.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 1:38 PM
 */

#ifndef TICKET_H
#define TICKET_H

#if     (INDEX_STRUCT == IDX_TICKET)
#elif   (INDEX_STRUCT == IDX_TICKET_PAD) 
#define PAD 
#define PAD_SIZE 24
#elif   (INDEX_STRUCT == IDX_TICKET_BASELINE) 
#define BASELINE
#else
#error
#endif

#ifdef BASELINE //make sure no padding is defined
    #ifdef PAD
      #undef PAD
    #endif
#endif

/*Other Definitions*/
#define ALIGNED(N) __attribute__ ((aligned (N)))
 
#define CACHE_LINE_SIZE 64

#if !defined(UNUSED)
#  define UNUSED __attribute__ ((unused))
#endif

#define MEM_BARRIER //nop on the opteron for these benchmarks

//atomic operations interface
//Compare-and-swap
#  define CAS_PTR(a,b,c) __sync_val_compare_and_swap(a,b,c)
#  define CAS_U8(a,b,c) __sync_val_compare_and_swap(a,b,c)
#  define CAS_U16(a,b,c) __sync_val_compare_and_swap(a,b,c)
#  define CAS_U32(a,b,c) __sync_val_compare_and_swap(a,b,c)
#  define CAS_U64(a,b,c) __sync_val_compare_and_swap(a,b,c)

#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)

#if !defined(COMPILER_BARRIER)
#  define COMPILER_BARRIER() asm volatile ("" ::: "memory")
#endif



static volatile int stop;

typedef union tl32
{
  struct
  {
    volatile uint16_t version;
    volatile uint16_t ticket;
  };
  volatile uint32_t to_uint32;
} tl32_t;


typedef union tl
{
  tl32_t lr[2];
  uint64_t to_uint64;
} tl_t;

static inline int
tl_trylock_version(volatile tl_t* tl, volatile tl_t* tl_old, int right)
{
  uint16_t version = tl_old->lr[right].version;
  uint16_t one = (uint16_t) 1;
  if (unlikely(version != tl_old->lr[right].ticket))
    {
      return 0;
    }

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 6
  tl32_t tlo = { {.version = version, .ticket = version} }; //{ .version = version, .ticket = version}; 
  tl32_t tln = { {.version = version, .ticket = (uint16_t) (version + one)} }; //{.version = version, .ticket = (version + 1)};
  return CAS_U32(&tl->lr[right].to_uint32, tlo.to_uint32, tln.to_uint32) == tlo.to_uint32;
#else
  tl32_t tlo = { version, version };
  tl32_t tln = { version, (version + 1) };
#endif
  return CAS_U32(&tl->lr[right].to_uint32, tlo.to_uint32, tln.to_uint32) == tlo.to_uint32;
}

#define TLN_REMOVED  0x0000FFFF0000FFFF0000LL

static inline int
tl_trylock_version_both(volatile tl_t* tl, volatile tl_t* tl_old)
{
  uint16_t v0 = tl_old->lr[0].version;
  uint16_t v1 = tl_old->lr[1].version;
  if (unlikely(v0 != tl_old->lr[0].ticket || v1 != tl_old->lr[1].ticket))
    {
      return 0;
    }

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 6
  tl_t tlo = { .to_uint64 = tl_old->to_uint64 };
  return CAS_U64(&tl->to_uint64, tlo.to_uint64, TLN_REMOVED) == tlo.to_uint64;
#else
  /* tl_t tlo; */
  /* tlo.uint64_t = tl_old->to_uint64; */
  uint64_t tlo = *(uint64_t*) tl_old;

  return CAS_U64((uint64_t*) tl, tlo, TLN_REMOVED) == tlo;
#endif

}


static inline void
tl_unlock(volatile tl_t* tl, int right)
{
  /* PREFETCHW(tl); */
  COMPILER_BARRIER;
  tl->lr[right].version++;
  COMPILER_BARRIER;
}

static inline void
tl_revert(volatile tl_t* tl, int right)
{
  /* PREFETCHW(tl); */
  COMPILER_BARRIER;
  tl->lr[right].ticket--;
  COMPILER_BARRIER;
}



template <typename skey_t, typename sval_t>
struct node_t
{
  skey_t key;
  #ifndef NO_VAL
  union
  {
    sval_t val;
    volatile uint64_t leaf;
  };
  #endif
  #if defined(BASELINE) || defined(NO_VOLATILE)
  volatile struct node_t<skey_t, sval_t>* left;
  volatile struct node_t<skey_t, sval_t>* right;
  #else
   struct node_t<skey_t, sval_t>* volatile left;
   struct node_t<skey_t, sval_t>* volatile right;
  #endif 
  volatile tl_t lock;
  #ifdef PAD
    char pad[PAD_SIZE];
  #endif
  
  #ifdef BASELINE
    uint8_t padding[CACHE_LINE_SIZE - 40];
  #endif
};

static itemid_t t_SpecialNull;
static sval_t SpecialNull = &t_SpecialNull;

template <typename skey_t, typename sval_t>
struct intset_t //used to have align argument 
{
  node_t<skey_t, sval_t>* head;
};

template <typename skey_t, typename sval_t, class RecMgr>
class ticket {
private:
    
    intset_t<skey_t, sval_t>* set; 
    
    node_t<skey_t, sval_t>* new_node(skey_t key, sval_t val, node_t<skey_t, sval_t>* l, node_t<skey_t, sval_t>* r, int initializing);
    node_t<skey_t, sval_t>* new_node_no_init();
    intset_t<skey_t, sval_t>* set_new();
    void set_delete(intset_t<skey_t, sval_t>* set);
    void node_delete(node_t<skey_t, sval_t>* node);
    sval_t bst_tk_delete(intset_t<skey_t, sval_t>* set, skey_t key);
    sval_t bst_tk_find(intset_t<skey_t, sval_t>* set, skey_t key);
    sval_t bst_tk_insert(intset_t<skey_t, sval_t>* set, skey_t key, sval_t val);

public:
    
    ticket() {
       set = set_new();
    }

    ~ticket() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        return bst_tk_insert(set,key,item);
    }

    sval_t find(skey_t key) {
        return  bst_tk_find(set,key);;
    }

    node_t<skey_t, sval_t> * get_root() {
        return set->head;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *) curr->left;
    }

    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr) {
        return (node_t<skey_t, sval_t> *) curr->right;
    }
};    
#endif /* TICKET_H */

