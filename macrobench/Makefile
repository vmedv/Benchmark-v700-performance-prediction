CFLAGS =
LDFLAGS =
CC=g++

include ../plaf.inc
CFLAGS += -DMAX_THREADS_POW2=$(thread_count_upper_bound_pow2)
CFLAGS += -DCPU_FREQ_GHZ=$(cpu_freq_ghz) -DCPU_FREQ=$(cpu_freq_ghz)
CFLAGS += -g -std=c++0x -O3
FLAGS += $(xargs)

workload=TPCC
data_structure_name=brown_ext_abtree_lf
data_structure_opts=
data_structure_path=../ds/$(data_structure_name)

.SUFFIXES: .o .cpp .h
bindir=bin
odir=$(bindir)/OBJS_$(workload)_$(data_structure_name)/macrobench
SRC_DIRS = ./ ./benchmarks/ ./concurrency_control/ ./storage/ ./storage/index/ ./system/ ../common/rlu/
CFLAGS += -I../ $(patsubst %,-I%,$(SRC_DIRS)) `find ../common -type d | sed s/^/-I/`

#CFLAGS += -DSKIP_PERMUTATIONS
CFLAGS += -DNOGRAPHITE=1 -DWORKLOAD=$(workload)
CFLAGS += -DHASH_PRIMARY_KEYS
#CFLAGS += -DALIGNED_ALLOCATIONS
CFLAGS += -I$(data_structure_path) $(data_structure_opts)
LDFLAGS += $(CFLAGS)
LDFLAGS += -L. -L../lib -pthread -g -lrt -ldl

CPPS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)*.cpp))
OBJS = $(foreach obj, $(CPPS:.cpp=.o), $(odir)/$(obj))
dir_guard=@mkdir -p $(@D)

all: $(bindir)/rundb_$(workload)_$(data_structure_name)

$(bindir)/rundb_$(workload)_$(data_structure_name): $(OBJS)
	$(dir_guard)
	$(CC) -o $@ $^ $(LDFLAGS)

$(odir)/%.o: %.cpp
	$(dir_guard)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@rm -f $(bindir)/rundb_$(workload)_$(data_structure_name)
	@rm -r -f $(odir)
