dict=CITRUS_SPIN
workload=TPCC

CC=g++
CFLAGS=-g -std=c++0x
#CFLAGS += -Wall
#CFLAGS += -DNDEBUG

.SUFFIXES: .o .cpp .h

ifeq ($(readonly),)
    workload2=$(workload)
else
    workload2=$(workload)_readonly
endif

machine=$(shell hostname)
bindir=bin/$(machine)
odir=$(bindir)/OBJS_$(workload2)_$(dict)

SRC_DIRS = ./ ./benchmarks/ ./concurrency_control/ ./storage/ ./storage/index/ ./system/
SRC_DIRS += ./rlu/
INCLUDE = -I. -I./include -I./benchmarks -I./concurrency_control -I./storage -I./system -I./storage/index

######################## DEFINES FOR RQ_DATA STRUCTURES ########################
#CFLAGS += -Winline
CFLAGS += -DNDEBUG
#CFLAGS += -DNO_FREE
#CFLAGS += -DUSE_STL_HASHLIST
CFLAGS += -DUSE_SIMPLIFIED_HASHLIST
#CFLAGS += -DRAPID_RECLAMATION
#CFLAGS += -DRWLOCK_PTHREADS
#CFLAGS += -DRWLOCK_FAVOR_WRITERS
CFLAGS += -DRWLOCK_FAVOR_READERS
#CFLAGS += -DSNAPCOLLECTOR_PRINT_RQS
#CFLAGS += -DUSE_RQ_DEBUGGING
#CFLAGS += -DRQ_VALIDATION
#CFLAGS += -DRQ_VISITED_IN_BAGS_HISTOGRAM
#CFLAGS += -DRQ_HISTOGRAM
#CFLAGS += -DADD_DELAY_BEFORE_DTIME
#CFLAGS += -DUSE_DEBUGCOUNTERS
#CFLAGS += -DRANDOMIZED_ALLOCATIONS
#CFLAGS += -DEXTRA_CACHE_MISSES_PER_OP
#CFLAGS += -DBLOCKBAG_ITERATOR_COUNT_BLOCKS_TRAVERSED
#CFLAGS += -DMIN_NODE_SIZE=48
CFLAGS += -DUSE_SIMPLIFIED_ABTREE_REBALANCING
#CFLAGS += -DBROWN_EXT_BST_LF_COLOCATE_MARKED_BIT
CFLAGS += -DABTREE_DEGREE=16
CFLAGS += -DPHYSICAL_PROCESSORS=64 -DMAX_TID_POW2=64
#CFLAGS += -DDEBUG=if\(1\) -DDEBUG1=if\(1\) -DDEBUG2=if\(1\)
#CFLAGS += -DDEBUG=if\(1\) -DDEBUG1=if\(0\) -DDEBUG2=if\(0\)
CFLAGS += -DDEBUG=if\(0\) -DDEBUG1=if\(0\) -DDEBUG2=if\(0\)
CFLAGS += -DMEMORY_STATS=if\(1\) -DMEMORY_STATS2=if\(1\)
#CFLAGS += -DMEMORY_STATS=if\(0\) -DMEMORY_STATS2=if\(0\)
CFLAGS += -DINSERT_FUNC=insertIfAbsent
#CFLAGS += -DUSE_PAPI
CFLAGS += $(xargs)

INCLUDE += -I../ds
INCLUDE += -I../ds/arbel_int_bst_rcu
INCLUDE += -I../ds/bronson_pext_bst_occ
INCLUDE += -I../ds/brown_ext_abtree_lf
INCLUDE += -I../ds/brown_ext_bst_lf
INCLUDE += -I../ds/herlihy_list_occ
INCLUDE += -I../ds/herlihy_list_lf
INCLUDE += -I../ds/herlihy_skiplist_occ
INCLUDE += -I../ds/matveev_int_bst_rlu
INCLUDE += -I../ds/matveev_list_rlu
INCLUDE += -I../ds/natarajan_ext_bst_lf
INCLUDE += -I../common
INCLUDE += -I../common/atomic_ops
INCLUDE += -I../common/dcss
INCLUDE += -I../common/descriptors
INCLUDE += -I../common/rlu
INCLUDE += -I../common/rq
INCLUDE += -I../common/rq/snapcollector
INCLUDE += -I../common/recordmgr
INCLUDE += -I../include

CFLAGS += -DINSERT_FUNC=insertIfAbsent
#CFLAGS += -DSKIP_PERMUTATIONS
CFLAGS += $(INCLUDE) -DNOGRAPHITE=1 -O3 -DINDEX_STRUCT=IDX_$(dict) -DWORKLOAD=$(workload) #-Werror
CFLAGS += -DSEGREGATE_MALLOC
CFLAGS += $(readonly)
#CFLAGS += -DREAD_ONLY
#CFLAGS += -DVERBOSE_1
CFLAGS += -DHASH_PRIMARY_KEYS
CFLAGS += -DALIGNED_ALLOCATIONS
#CFLAGS += -DINDEX_NO_RECLAMATION
LDFLAGS = -L. -L./libs -pthread -g -lrt -std=c++0x -O3 -ldl
#LDFLAGS += -ljemalloc -Wl,-rpath,../lib
#LDFLAGS += -ltcmalloc
LDFLAGS += $(CFLAGS)

CPPS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)*.cpp))
OBJS = $(foreach obj, $(CPPS:.cpp=.o), $(odir)/$(obj))

dir_guard=@mkdir -p $(@D)

.PHONY: all clean
all: $(bindir)/rundb_$(workload2)_$(dict)

$(bindir)/rundb_$(workload2)_$(dict): $(OBJS)
	$(dir_guard)
	$(CC) -o $@ $^ $(LDFLAGS)

$(odir)/%.o: %.cpp
	$(dir_guard)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@rm -f $(bindir)/rundb_$(workload2)_$(dict)
	@rm -r -f $(odir)
