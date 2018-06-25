FLAGS =
LDFLAGS =
GPP = g++

### if you do not have PAPI, comment out these two lines
FLAGS += -DUSE_PAPI
LDFLAGS += -lpapi

include ../plaf.inc
FLAGS += -DMAX_THREADS_POW2=$(thread_count_upper_bound_pow2)
FLAGS += -DCPU_FREQ_GHZ=$(cpu_freq_ghz) -DCPU_FREQ=$(cpu_freq_ghz)
FLAGS += -DMEMORY_STATS=if\(1\) -DMEMORY_STATS2=if\(0\)
FLAGS += -std=c++11 -mcx16 -O3 -g
FLAGS += -fno-omit-frame-pointer
FLAGS += -fopenmp
FLAGS += -DNO_DELETE_DS
#FLAGS += -DRAPID_RECLAMATION
#FLAGS += -DPREFILL_INSERTION_ONLY
FLAGS += -DNDEBUG
FLAGS += $(xargs)

LDFLAGS += -L../lib
LDFLAGS += -I./ -I../ `find ../common -type d | sed s/^/-I/`
LDFLAGS += -lpthread
LDFLAGS += -ldl

MINIMAL_CPP=$(wildcard ../ds/*/minimal_example.cpp)
MINIMAL_BINS=$(patsubst ../ds/%/minimal_example.cpp,%.minimal,$(MINIMAL_CPP))

DATA_STRUCTURE_ADAPTERS_H=$(wildcard ../ds/*/adapter.h)

UBENCH_RNONE_BINS=$(patsubst ../ds/%/adapter.h,%.ubench_rnone,$(DATA_STRUCTURE_ADAPTERS_H))
UBENCH_RDEBRA_BINS=$(patsubst ../ds/%/adapter.h,%.ubench_rdebra,$(DATA_STRUCTURE_ADAPTERS_H))
UBENCH_RDEBRACAP_BINS=$(patsubst ../ds/%/adapter.h,%.ubench_rdebracap,$(DATA_STRUCTURE_ADAPTERS_H))

bin_dir=bin

all: ubench
#all: minimal

ubench: $(UBENCH_RNONE_BINS) $(UBENCH_RDEBRA_BINS) $(UBENCH_RDEBRACAP_BINS)
minimal: $(MINIMAL_BINS)

%.minimal:
	$(GPP) $(FLAGS) ../ds/$(@:%.minimal=%)/minimal_example.cpp -o $(bin_dir)/$@ $(LDFLAGS)

%.ubench_rnone:
	$(GPP) $(FLAGS) -o $(bin_dir)/$@ -I../ds/$(@:%.ubench_rnone=%) -DUSE_TREE_STATS -DDS_TYPENAME=$(@:%.ubench_rnone=%) main.cpp -DRECLAIM_TYPE=none $(LDFLAGS)
%.ubench_rdebra:
	$(GPP) $(FLAGS) -o $(bin_dir)/$@ -I../ds/$(@:%.ubench_rdebra=%) -DUSE_TREE_STATS -DDS_TYPENAME=$(@:%.ubench_rdebra=%) main.cpp -DRECLAIM_TYPE=debra $(LDFLAGS)
%.ubench_rdebracap:
	$(GPP) $(FLAGS) -o $(bin_dir)/$@ -I../ds/$(@:%.ubench_rdebracap=%) -DUSE_TREE_STATS -DDS_TYPENAME=$(@:%.ubench_rdebracap=%) main.cpp -DRECLAIM_TYPE=numa_ebr $(LDFLAGS)

clean:
	rm $(bin_dir)/*
