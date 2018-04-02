FLAGS =
LDFLAGS =
GPP = g++

include ../plaf.inc
FLAGS += -DMAX_THREADS_POW2=$(thread_count_upper_bound_pow2)
FLAGS += -DCPU_FREQ_GHZ=$(cpu_freq_ghz) -DCPU_FREQ=$(cpu_freq_ghz)
FLAGS += -DMEMORY_STATS=if\(1\) -DMEMORY_STATS2=if\(0\)
FLAGS += -std=c++11 -mcx16 -O3 -g
FLAGS += -fno-omit-frame-pointer
FLAGS += -DRAPID_RECLAMATION
FLAGS += -DNO_DELETE_DS
FLAGS += -DRECLAIM_TYPE=tree_ebr
#FLAGS += -DRECLAIM_TYPE=numa_ebr
#FLAGS += -DRECLAIM_TYPE=debra
#FLAGS += -fprefetch-loop-arrays
#FLAGS += -DNDEBUG
FLAGS += $(xargs)

LDFLAGS += -L../lib
LDFLAGS += -I./ -I../ `find ../common -type d | sed s/^/-I/`

LDFLAGS += -lpthread
LDFLAGS += -ldl

#FLAGS += -DUSE_PAPI
#LDFLAGS += -lpapi

MINIMAL_CPP=$(wildcard ../ds/*/minimal_example.cpp)
MINIMAL_BINS=$(patsubst ../ds/%/minimal_example.cpp,%.minimal,$(MINIMAL_CPP))

DATA_STRUCTURE_ADAPTERS_H=$(wildcard ../ds/*/adapter.h)
UBENCH_BINS=$(patsubst ../ds/%/adapter.h,%.ubench,$(DATA_STRUCTURE_ADAPTERS_H))

bin_dir=bin

all: ubench minimal
ubench: $(UBENCH_BINS)
minimal: $(MINIMAL_BINS)

%.minimal:
	$(GPP) $(FLAGS) ../ds/$(@:%.minimal=%)/minimal_example.cpp -o $(bin_dir)/$@ $(LDFLAGS)

%.ubench:
	$(GPP) $(FLAGS) -o $(bin_dir)/$@ -I../ds/$(@:%.ubench=%) main.cpp $(LDFLAGS)

clean:
	rm $(bin_dir)/*
