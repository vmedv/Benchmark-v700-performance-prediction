# WORKLOADS

## Types of workloads

### standard 

A standard workload where there is only one key allocation for all types of operations. 

To call it, the `workload_type` argument must be left empty and `distribution_type` specified.

### Skewed sets

The argument to call is `-skewed-sets`

Different distributions are used for read and write requests.

Notes:
+ During pre-filling, the hot reading set is filled first, and after that everything else is evenly distributed.
For classical filling, where the keys for insertion and deletion are generated in the same way as in normal operation, you need to add the `-write-prefill-only` parameter. 
+ At this stage of implementation, only `skewed-uniform` distributions are used

[//]: # (When using skewed distributions, )
[//]: # (to control the intersection of "hot" data for read and write operations )

### Temporary skewed

The argument to call is `-temporary-skewed` or `-temp-skewed`

Each time period has its own distribution of keys. 
There are two types of time interval:
1. Hot time — when elements from a subset
are called more often than others
2. Rest time — when elements are called equally likely

Note:
+ At this stage of implementation, only `skewed-uniform` distributions are used

### Сreakers and wave

The argument to call is `-creakers-and-wave`

There are two types of data: oldies and wave.  
Oldies (creakers) — data that is rarely requested, but on an ongoing basis.  
Wave — data where new elements are requested very often,
but over time their demand decreases, after which they are completely deleted.

Note: 
+ The `-prefillsize` parameter is ignored. The structure is filled with old men and the first wave.
+ If the data is deleted logically first and physically over time,
the tester must select the value of the number of keys `N` so
that the element is guaranteed to be physically deleted in `N` requests.
Otherwise, there is a high probability that the data from `wave` will become the same oldies,
and the essence of the workload will disappear.

## Parameters

### Notation

+ `<p>` is an integer taking the value [0, 100]
+ `<n>` is an integer taking the value [0, +∞]
+ `<f>` is a fractional number taking the value [0, 1]
+ `<d>` is a fractional number taking the value [0, +∞]

[//]: # (+ `<p>` —)

[//]: # ($+\infty$+∞)


### General parameters 

+ `-i <f>` — percentage of insert operations
+ `-d <f>` — percentage of delete operations
+ `-insdel <f1> <f2>` — percentage of insert and delete operations. 
Equivalent to `-i <f1> -d <f2>` 
+ `-rq <>` — todo is probably related to queries in the interval
+ `-rqsize <>` —
+ `-k <n>` — number of keys 
+ `-nrq <>` —
+ `-nwork <n>` — number of threads
+ `-nprefill <n>` — the number of allocated threads for pre-filling 
+ `-prefill-insert' — filling the container with insert operations 
+ `-prefill-mixed` —
+ `-prefill-hybrid-min-ms <>` —
+ `-prefill-hybrid-max-ms <>` —
+ `-prefillsize <n>` — number of keys to fill in 
+ `-t <n>` — the time of the benchmark in milliseconds 
+ `-pin <....>` —

### Standard workload

+ `<dist_type>` — the distribution
that will be used in the standard workload.

  
### Skewed sets

+ `-rs <f>` — the size of the "hot" data to read in relation to the entire set of keys
+ `-ws <f>` — similar for write operation
+ `-rp <f>` — probability of reading "hot" data
+ `-wp <f>` — similar for write operation
+ `-inter <f>` — the size of the intersection of the "hot" data for reading and writing
in relation to the entire set of keys. Important condition: `inter <= rs` and `inter <= ws`
+ `-write-prefill-only` — changes the type of key generation for pre-filling.

### Temporary skewed

+ `-set-count <n>` — the number of time-dependent subsets/distributions
+ `-ht <h>` — default hot time (specified in the number of iterations):
if the hot time after a certain time interval is not explicitly specified,
the value of this parameter will be used
+ `-rt <n>` — default rest time (specified in the number of iterations):
if the rest time after a certain hot time interval is not explicitly specified,
the value of this parameter will be used
+ `-si <n> <f>` — size of hot data from the `nth` subset 
in relation to the entire set of keys
+ `-pi <n> <f>` — the probability of accessing hot data from the `nth` subset
in relation to the entire set of keys
+ `-hti <n1> <n2>` — working time with hot data from the `n1' subset
+ `-rti <n1> <n2>` — rest time after working with hot data from the `n1' subset

#### Extra parameters

+ `-non-shuffle` — enabling the mode without mixing keys
+ `-sbi' <n> <f> is the left boundary of the hot data from the `nth` subset
  in relation to the entire set of keys (works only when the `non-shuffle` mode is enabled)

### Creakers and wave

+ `-gs <f>` ||`-cs <f>` — the size of the subset of oldies
in relation to the entire set of keys
+ `-gp <f>` || `-cp <f>` — probability of reading from a set of oldies
+ `-ws <f>` — the size of the wave in relation to the entire set of keys
+ `-g-age <n>` ||`-c-age <n>` — the age of old people. 
Before testing starts, `n` read requests will be made 
of the many old men
+ `-g-dist <dist_type>` || `-c-dist <dist_type>` — distribution of keys
for a lot of oldies. 
(Only uniform distribution is available so far)
+ `-w-dist <dist_type>` — key distribution
for the wave set. (Only the Zipf distribution is available so far)

### Distributions

+ `-dist-zipf <d>` — Zipf distribution with parameter `d`
+ `-dist-uniform` — uniform distribution
+ `-dist-skewed-uniform ...` — skewed distribution
