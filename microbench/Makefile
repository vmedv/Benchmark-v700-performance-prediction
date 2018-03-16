FLAGS =
LDFLAGS =
GPP = g++

FLAGS += -DMAX_THREADS_POW2=256 -DCPU_FREQ_GHZ=2.1
FLAGS += -std=c++11 -mcx16 -O3 -g
FLAGS += -DNDEBUG
FLAGS += $(xargs)

LDFLAGS += -L../lib
LDFLAGS += -I./ -I../ `find ../common -type d | sed s/^/-I/`

LDFLAGS += -lpthread
LDFLAGS += -ldl

#FLAGS += -DUSE_PAPI
#LDFLAGS += -lpapi

ds =
ds += natarajan_ext_bst_lf
ds += brown_ext_bst_lf
ds += brown_ext_abtree_lf
ds += bronson_pext_bst_occ
ds += brown_ext_bslack_lf

all: ubench minimal
ubench: $(ds:%=%.ubench)	
minimal: $(ds:%=%.minimal)

%.minimal:
	$(GPP) $(FLAGS) ../ds/$(@:%.minimal=%)/minimal_example.cpp -o $@.out $(LDFLAGS)

%.ubench:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE=ds/$(@:%.ubench=%)/$(@:%.ubench=%)_adapter.h main.cpp $(LDFLAGS)






























natarajan_ext_bst_lf.baseline:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_baseline_impl.h" -DBASELINE main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage0:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_baseline_impl.h" main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage1:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" -DDS_H_FILE="natarajan_ext_bst_lf_stage1_impl.h" main.cpp $(LDFLAGS)
natarajan_ext_bst_lf.stage2:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/natarajan_ext_bst_lf/natarajan_ext_bst_lf_adapter.h" main.cpp $(LDFLAGS)

brown_ext_bst_lf.reclaim_debra.nopool:
	$(GPP) $(FLAGS) -o $@.out -DADAPTER_H_FILE="ds/brown_ext_bst_lf/brown_ext_bst_lf_adapter.h" -DRQ_UNSAFE -DRECLAIM_TYPE=debra -DALLOC_TYPE=new -DPOOL_TYPE=none main.cpp $(LDFLAGS)

rlulist:
	$(GPP) $(FLAGS) -o lazylist.rq_rlu.out -DRLU_LIST main.cpp ../common/rlu/rlu.cpp $(LDFLAGS)
rlucitrus:
	$(GPP) $(FLAGS) -o citrus.rq_rlu.out -DRLU_CITRUS main.cpp ../common/rlu/rlu.cpp $(LDFLAGS)
