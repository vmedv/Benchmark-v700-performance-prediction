FLAGS =
LDFLAGS =
GPP = g++

### if you do not have PAPI, comment out these two lines
#FLAGS += -DUSE_PAPI
#LDFLAGS += -lpapi

include ../plaf.inc
FLAGS += -DMAX_THREADS_POW2=$(thread_count_upper_bound_pow2)
FLAGS += -DCPU_FREQ_GHZ=$(cpu_freq_ghz) -DCPU_FREQ=$(cpu_freq_ghz)
FLAGS += -DMEMORY_STATS=if\(1\) -DMEMORY_STATS2=if\(0\)
FLAGS += -std=c++11 -mcx16 -O3 -g
FLAGS += -fno-omit-frame-pointer
FLAGS += -fopenmp
FLAGS += -DNO_DELETE_DS
#FLAGS += -DRAPID_RECLAMATION
FLAGS += -DPREFILL_INSERTION_ONLY
FLAGS += -DMEASURE_REBUILDING_TIME
FLAGS += -DDEBRA_DISABLE_READONLY_OPT
FLAGS += -DUSE_TREE_STATS
#FLAGS += -DOVERRIDE_PRINT_STATS_ON_ERROR
#FLAGS += -DNDEBUG
FLAGS += -Wno-format
FLAGS += $(xargs)

LDFLAGS += -L../lib
LDFLAGS += -I./ -I../ `find ../common -type d | sed s/^/-I/`
LDFLAGS += -lpthread
LDFLAGS += -ldl
LDFLAGS += -lnuma

bin_dir=bin
dir_guard:
	@mkdir -p $(bin_dir)

MINIMAL_BINS=$(patsubst ../ds/%/minimal_example.cpp,%.minimal,$(wildcard ../ds/*/minimal_example.cpp))
minimal_%: dir_guard
	$(GPP) $(FLAGS) ../ds/$(@:minimal_%=%)/minimal_example.cpp -o $(bin_dir)/$@ $(LDFLAGS)

DATA_STRUCTURE_ADAPTERS_H=$(wildcard ../ds/*/adapter.h)
DATA_STRUCTURES=$(patsubst ../ds/%/adapter.h,%,$(DATA_STRUCTURE_ADAPTERS_H))

RECLAIMERS=none debra ebr_token ebr_tree
POOLS=none numa
ALLOCATORS=new

define make-custom-target =
bin=ubench_$(1).alloc_$(2).reclaim_$(3).pool_$(4)
$(bin): dir_guard
	$(GPP) main.cpp -o $(bin_dir)/$(bin) -I../ds/$1 -DDS_TYPENAME=$(1) -DALLOC_TYPE=$(2) -DRECLAIM_TYPE=$(3) -DPOOL_TYPE=$(4) $(FLAGS) $(LDFLAGS)
all:: $(bin)
endef

$(foreach alg,$(DATA_STRUCTURES),$(foreach a,$(ALLOCATORS),$(foreach r,$(RECLAIMERS),$(foreach p,$(POOLS),$(eval $(call make-custom-target,$(alg),$(a),$(r),$(p)))))))

clean:
	rm $(bin_dir)/*
