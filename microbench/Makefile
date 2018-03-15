FLAGS =
LDFLAGS =
GPP = g++

FLAGS += -DMAX_THREADS_POW2=64 -DCPU_FREQ_GHZ=2.1
FLAGS += -std=c++11 -mcx16 -O3 -g
FLAGS += $(xargs)

LDFLAGS += -L../lib
LDFLAGS += -I./ -I../ `find ../common -type d | sed s/^/-I/`

LDFLAGS += -lpthread
LDFLAGS += -ldl

#FLAGS += -DUSE_PAPI
#LDFLAGS += -lpapi

#all: natarajan_ext_bst_lf
all: minimal_examples

abtree: abtree.rq_lockfree abtree.rq_rwlock abtree.rq_htm_rwlock abtree.rq_unsafe abtree.rq_unsafe_ts
abtree.rq_lockfree:
	$(GPP) $(FLAGS) -o $@.out -DABTREE -DRQ_LOCKFREE main.cpp $(LDFLAGS)
abtree.rq_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DABTREE -DRQ_RWLOCK main.cpp $(LDFLAGS)
abtree.rq_htm_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DABTREE -DRQ_HTM_RWLOCK main.cpp $(LDFLAGS)
abtree.rq_unsafe:
	$(GPP) $(FLAGS) -o $@.out -DABTREE -DRQ_UNSAFE main.cpp $(LDFLAGS)
abtree.rq_unsafe_ts:
	$(GPP) $(FLAGS) -o $@.out -DABTREE -DRQ_UNSAFE -DRQ_USE_TIMESTAMPS main.cpp $(LDFLAGS)

lazylist: lazylist.rq_lockfree lazylist.rq_rwlock lazylist.rq_htm_rwlock lazylist.rq_unsafe lazylist.rq_unsafe_ts
lazylist.rq_lockfree:
	$(GPP) $(FLAGS) -o $@.out -DLAZYLIST -DRQ_LOCKFREE main.cpp $(LDFLAGS)
lazylist.rq_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DLAZYLIST -DRQ_RWLOCK main.cpp $(LDFLAGS)
lazylist.rq_htm_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DLAZYLIST -DRQ_HTM_RWLOCK main.cpp $(LDFLAGS)
lazylist.rq_unsafe:
	$(GPP) $(FLAGS) -o $@.out -DLAZYLIST -DRQ_UNSAFE main.cpp $(LDFLAGS)
lazylist.rq_unsafe_ts:
	$(GPP) $(FLAGS) -o $@.out -DLAZYLIST -DRQ_UNSAFE -DRQ_USE_TIMESTAMPS main.cpp $(LDFLAGS)

lflist: lflist.rq_lockfree lflist.rq_rwlock lflist.rq_htm_rwlock lflist.rq_unsafe lflist.rq_unsafe_ts lflist.rq_snapcollector
lflist.rq_lockfree:
	$(GPP) $(FLAGS) -o $@.out -DLFLIST -DRQ_LOCKFREE main.cpp $(LDFLAGS)
lflist.rq_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DLFLIST -DRQ_RWLOCK main.cpp $(LDFLAGS)
lflist.rq_htm_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DLFLIST -DRQ_HTM_RWLOCK main.cpp $(LDFLAGS)
lflist.rq_unsafe:
	$(GPP) $(FLAGS) -o $@.out -DLFLIST -DRQ_UNSAFE main.cpp $(LDFLAGS)
lflist.rq_unsafe_ts:
	$(GPP) $(FLAGS) -o $@.out -DLFLIST -DRQ_UNSAFE -DRQ_USE_TIMESTAMPS main.cpp $(LDFLAGS)
lflist.rq_snapcollector:
	$(GPP) $(FLAGS) -o $@.out -DLFLIST -DRQ_SNAPCOLLECTOR main.cpp $(LDFLAGS)

skiplistlock: skiplistlock.rq_lockfree skiplistlock.rq_rwlock skiplistlock.rq_htm_rwlock skiplistlock.rq_unsafe skiplistlock.rq_unsafe_ts skiplistlock.rq_snapcollector
skiplistlock.rq_lockfree:
	$(GPP) $(FLAGS) -o $@.out -DSKIPLISTLOCK -DRQ_LOCKFREE main.cpp $(LDFLAGS)
skiplistlock.rq_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DSKIPLISTLOCK -DRQ_RWLOCK main.cpp $(LDFLAGS)
skiplistlock.rq_htm_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DSKIPLISTLOCK -DRQ_HTM_RWLOCK main.cpp $(LDFLAGS)
skiplistlock.rq_unsafe:
	$(GPP) $(FLAGS) -o $@.out -DSKIPLISTLOCK -DRQ_UNSAFE main.cpp $(LDFLAGS)
skiplistlock.rq_unsafe_ts:
	$(GPP) $(FLAGS) -o $@.out -DSKIPLISTLOCK -DRQ_UNSAFE -DRQ_USE_TIMESTAMPS main.cpp $(LDFLAGS)
skiplistlock.rq_snapcollector:
	$(GPP) $(FLAGS) -o $@.out -DSKIPLISTLOCK -DRQ_SNAPCOLLECTOR main.cpp $(LDFLAGS)

bst: bst.rq_lockfree bst.rq_rwlock bst.rq_htm_rwlock bst.rq_unsafe bst.rq_unsafe_ts
bst.rq_lockfree:
	$(GPP) $(FLAGS) -o $@.out -DBST -DRQ_LOCKFREE main.cpp $(LDFLAGS)
bst.rq_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DBST -DRQ_RWLOCK main.cpp $(LDFLAGS)
bst.rq_htm_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DBST -DRQ_HTM_RWLOCK main.cpp $(LDFLAGS)
bst.rq_unsafe:
	$(GPP) $(FLAGS) -o $@.out -DBST -DRQ_UNSAFE main.cpp $(LDFLAGS)
bst.rq_unsafe_ts:
	$(GPP) $(FLAGS) -o $@.out -DBST -DRQ_UNSAFE -DRQ_USE_TIMESTAMPS main.cpp $(LDFLAGS)

citrus: citrus.rq_lockfree citrus.rq_rwlock citrus.rq_htm_rwlock citrus.rq_unsafe citrus.rq_unsafe_ts
citrus.rq_lockfree:
	$(GPP) $(FLAGS) -o $@.out -DCITRUS -DRQ_LOCKFREE main.cpp $(LDFLAGS)
citrus.rq_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DCITRUS -DRQ_RWLOCK main.cpp $(LDFLAGS)
citrus.rq_htm_rwlock:
	$(GPP) $(FLAGS) -o $@.out -DCITRUS -DRQ_HTM_RWLOCK main.cpp $(LDFLAGS)
citrus.rq_unsafe:
	$(GPP) $(FLAGS) -o $@.out -DCITRUS -DRQ_UNSAFE main.cpp $(LDFLAGS)
citrus.rq_unsafe_ts:
	$(GPP) $(FLAGS) -o $@.out -DCITRUS -DRQ_UNSAFE -DRQ_USE_TIMESTAMPS main.cpp $(LDFLAGS)

rlu: rlulist rlucitrus
rlulist:
	$(GPP) $(FLAGS) -o lazylist.rq_rlu.out -DRLU_LIST main.cpp ../common/rlu/rlu.cpp $(LDFLAGS)
rlucitrus:
	$(GPP) $(FLAGS) -o citrus.rq_rlu.out -DRLU_CITRUS main.cpp ../common/rlu/rlu.cpp $(LDFLAGS)

minimal_examples: brown_ext_abtree_lf.minimal_example brown_ext_bst_lf.minimal_example
brown_ext_abtree_lf.minimal_example:
	$(GPP) $(FLAGS) ../ds/brown_ext_abtree_lf/minimal_example.cpp -o $@.out $(LDFLAGS)
brown_ext_bst_lf.minimal_example:
	$(GPP) $(FLAGS) ../ds/brown_ext_bst_lf/minimal_example.cpp -o $@.out $(LDFLAGS)

natarajan_ext_bst_lf: natarajan_ext_bst_lf.baseline natarajan_ext_bst_lf.stage0 natarajan_ext_bst_lf.stage1 natarajan_ext_bst_lf.stage2
natarajan_ext_bst_lf.baseline:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_baseline_impl.h" -DBASELINE main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage0:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_baseline_impl.h" main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage1:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage1_impl.h" main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage2:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage2_impl.h" main.cpp $(LDFLAGS)

bronson_pext_bst_occ.full: bronson_pext_bst_occ.reclaim_debra.nopool bronson_pext_bst_occ.reclaim_none.nopool bronson_pext_bst_occ.reclaim_debra.pool bronson_pext_bst_occ.reclaim_none.pool
bronson_pext_bst_occ.reclaim_debra.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/bronson_pext_bst_occ/bronson_pext_bst_occ_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)
bronson_pext_bst_occ.reclaim_none.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/bronson_pext_bst_occ/bronson_pext_bst_occ_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=none -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)
bronson_pext_bst_occ.reclaim_debra.pool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/bronson_pext_bst_occ/bronson_pext_bst_occ_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=perthread_and_shared main.cpp $(LDFLAGS)
bronson_pext_bst_occ.reclaim_none.pool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/bronson_pext_bst_occ/bronson_pext_bst_occ_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=none -DALLOC_TYPE=new -DPOOL_TYPE=perthread_and_shared main.cpp $(LDFLAGS)

brown_ext_bst_lf.full: brown_ext_bst_lf.reclaim_debra.nopool brown_ext_bst_lf.reclaim_none.nopool brown_ext_bst_lf.reclaim_debra.pool brown_ext_bst_lf.reclaim_none.pool
brown_ext_bst_lf.reclaim_debra.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/brown_ext_bst_lf/brown_ext_bst_lf_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)
brown_ext_bst_lf.reclaim_none.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/brown_ext_bst_lf/brown_ext_bst_lf_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=none -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)
brown_ext_bst_lf.reclaim_debra.pool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/brown_ext_bst_lf/brown_ext_bst_lf_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=perthread_and_shared main.cpp $(LDFLAGS)
brown_ext_bst_lf.reclaim_none.pool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/brown_ext_bst_lf/brown_ext_bst_lf_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=none -DALLOC_TYPE=new -DPOOL_TYPE=perthread_and_shared main.cpp $(LDFLAGS)

natarajan_ext_bst_lf.full: natarajan_ext_bst_lf.stage2.reclaim_debra.pool natarajan_ext_bst_lf.stage2.reclaim_debra.nopool natarajan_ext_bst_lf.stage2.reclaim_none.pool natarajan_ext_bst_lf.stage2.reclaim_none.nopool
natarajan_ext_bst_lf.stage2.reclaim_debra.pool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage2_impl.h" -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=perthread_and_shared main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage2.reclaim_debra.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage2_impl.h" -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage2.reclaim_none.pool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage2_impl.h" -DRECLAIM_TYPE=none -DALLOC_TYPE=new -DPOOL_TYPE=perthread_and_shared main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage2.reclaim_none.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage2_impl.h" -DRECLAIM_TYPE=none -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)
