CFLAGS =
LDFLAGS =
CC=g++

CFLAGS += -DMAX_THREADS_POW2=256 -DCPU_FREQ_GHZ=2.1
CFLAGS += -g -std=c++0x -O3
#CFLAGS += -DNDEBUG
FLAGS += $(xargs)

workload=TPCC
data_structure_name=brown_ext_abtree_lf
data_structure_opts=-DUSE_RANGE_QUERIES
data_structure_path=../ds/$(data_structure_name)

#data_structure_name=hash
#data_structure_opts=-DIDX_HASH

.SUFFIXES: .o .cpp .h
bindir=bin
odir=$(bindir)/OBJS_$(workload)_$(data_structure_name)
SRC_DIRS = ./ ./benchmarks/ ./concurrency_control/ ./storage/ ./storage/index/ ./system/ ../common/rlu/
CFLAGS += -I../ $(patsubst %,-I%,$(SRC_DIRS)) `find ../common -type d | sed s/^/-I/`

#CFLAGS += -DSKIP_PERMUTATIONS
CFLAGS += -DNOGRAPHITE=1 -DWORKLOAD=$(workload)
CFLAGS += -DHASH_PRIMARY_KEYS
CFLAGS += -DALIGNED_ALLOCATIONS
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
