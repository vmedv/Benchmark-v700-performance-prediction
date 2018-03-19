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

MINIMAL_CPP=$(wildcard ../ds/*/minimal_example.cpp)
MINIMAL_BINS=$(patsubst ../ds/%/minimal_example.cpp,%.minimal,$(MINIMAL_CPP))

DATA_STRUCTURE_ADAPTERS_H=$(wildcard ../ds/*/adapter.h)
UBENCH_BINS=$(patsubst ../ds/%/adapter.h,%.ubench,$(DATA_STRUCTURE_ADAPTERS_H))

all: ubench minimal
ubench: $(UBENCH_BINS)
minimal: $(MINIMAL_BINS)

%.minimal:
	$(GPP) $(FLAGS) ../ds/$(@:%.minimal=%)/minimal_example.cpp -o $@.out $(LDFLAGS)

%.ubench:
	$(GPP) $(FLAGS) -o $@.out -I../ds/$(@:%.ubench=%) main.cpp $(LDFLAGS)
