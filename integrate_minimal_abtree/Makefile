GPP = g++
FLAGS = -std=c++11 -mcx16 -O3 -g

LDFLAGS += -I./common
LDFLAGS += -I./common/dcss
LDFLAGS += -I./common/atomic_ops
LDFLAGS += -I./common/descriptors
LDFLAGS += -I./common/recordmgr
LDFLAGS += -I./common/rq
LDFLAGS += -I./common/rq/snapcollector
LDFLAGS += -I./ds/brown_ext_abtree_lf
LDFLAGS += -lpthread

all: minimal_example

minimal_example:
	$(GPP) $(FLAGS) -o $@.out $@.cpp $(LDFLAGS)
