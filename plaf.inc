## Note: as a convenience hack, this file is included in Makefiles,
##       AND in bash scripts, to set some variables.
##       So, don't write anything that isn't valid in both make AND bash.

## Thread counts to run in the experiments.
thread_counts="1 18 36 72 108 144"

## An upper bound on the number of threads that will be run. Must be a power of 2.
thread_count_upper_bound_pow2=256

## Nominal speed of the processor (for converting RDTSC into timing measurements on Intel).
cpu_freq_ghz=2.1

## Configure the thread pinning/binding policy.
## Blank means no thread pinning. (Threads can run wherever they want.)
pinning_policy="-pin 0-17.72-89.18-35.90-107.36-53.108-125.54-71.126-143"

jemalloc_file="lib/libjemalloc-5.0.1-25.so"
