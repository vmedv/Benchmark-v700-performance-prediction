This is a very preliminary README.

This is a benchmark designed to evaluate sets/dictionaries/maps with extremely low overhead, and powerful support for gathering statistics concerning the data structure and benchmark, both during and after an execution.

You should be able to clone and compile as follows.

git clone https://bitbucket.org/trbot86/setbench.git
cd setbench/microbench
make -j all

This will create many binaries in microbench/bin/. These can be run with command line arguments that can be seen in microbench/main.cpp, near the bottom of the file. Here is an example command that demonstrates the arguments. Starting in setbench/microbench:

./bin/ubench_natarajan_ext_bst_lf.alloc_new.reclaim_none.pool_none.out -nwork 64 -nprefill 64 -i 5 -d 5 -rq 0 -rqsize 1 -k 2000000 -nrq 0 -t 3000 -pin 0-15,32-47,16-31,48-63

This command will benchmark the lock-free Natarajan external binary search tree (with *no* memory reclamation) with 64 threads repeatedly performing 5% key-inserts and 5% key-deletes and 90% key-searches, on random keys from the key range [0, 2000000), for 3000 ms. The data structure is initially prefilled by 64 threads to contain half of the key range. The -pin argument causes threads to be pinned. The specified thread pinning order is for one 64 thread system. (Try "lscpu" for a reasonable pinning order.)

You can use LD_PRELOAD to load different memory allocators. (Using scalable allocator is crucial in evaluating concurrent data structures.) Setbench includes JEMalloc, Hoard and TCMalloc libraries in /lib/. For instance, you can plug JEMalloc into setbench, instead of the default allocator, by running:

LD_PRELOAD=../lib/libjemalloc.so ./bin/ubench_natarajan_ext_bst_lf.alloc_new.reclaim_none.pool_none.out -nwork 64 -nprefill 64 -i 5 -d 5 -rq 0 -rqsize 1 -k 2000000 -nrq 0 -t 3000 -pin 0-15,32-47,16-31,48-63
