FLAGS = 
FLAGS += -DMAX_THREADS_POW2=64
FLAGS += -DCPU_FREQ_GHZ=2.1

GPP = g++
FLAGS += -std=c++11 -mcx16 -O3
FLAGS += -g

FLAGS += -DNDEBUG
FLAGS += -DUSE_GSTATS
#FLAGS += -DNO_FREE
#FLAGS += -DUSE_STL_HASHLIST
FLAGS += -DUSE_SIMPLIFIED_HASHLIST
#FLAGS += -DRAPID_RECLAMATION
#FLAGS += -DRWLOCK_PTHREADS
#FLAGS += -DRWLOCK_FAVOR_WRITERS
FLAGS += -DRWLOCK_FAVOR_READERS
FLAGS += -DDEBUG=if\(0\) -DDEBUG1=if\(0\) -DDEBUG2=if\(0\)
FLAGS += -DMEMORY_STATS=if\(1\) -DMEMORY_STATS2=if\(1\)
FLAGS += -DINSERT_FUNC=insertIfAbsent
#FLAGS += -DUSE_PAPI
FLAGS += $(xargs)

LDFLAGS = -L../lib
LDFLAGS += -I../ds
LDFLAGS += -I../ds/brown_ext_abtree_lf
LDFLAGS += -I../common
LDFLAGS += -I../common/atomic_ops
LDFLAGS += -I../common/dcss
LDFLAGS += -I../common/descriptors
LDFLAGS += -I../common/rq
LDFLAGS += -I../common/rq/snapcollector
LDFLAGS += -I../common/recordmgr
LDFLAGS += -I../include
LDFLAGS += -I../ubench

LDFLAGS += -lpthread
#LDFLAGS += -lpapi

all:
	$(GPP) $(FLAGS) -o ubench.out -DABTREE_DEGREE=16 -DADAPTER_H_FILE="brown_ext_abtree_lf_adapter.h" -DRQ_UNSAFE ubench.cpp $(LDFLAGS)
