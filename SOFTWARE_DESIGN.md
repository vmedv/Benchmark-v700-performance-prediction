# Software Design

[Figure](#bench_uml) shows our flexible benchmark suite through a top-down design.
Each thread (gray box) is assigned its own ThreadLoop.
Each ThreadLoop, in turn, is assigned a set of configurations,
which correspond to the operations it will run (light blue box).
Each operation generates its arguments via a set of PRNGs, distributions over those PRNGs,
and mapping functions for converting the output of a distribution into a key or value.
Note that for simplicity, we depict a tree, but it is possible for a ThreadLoop to share a PRNG, DataMap,
or distribution among its blue boxes, and even for a read-only DataMap to be shared among ThreadLoops.

[//]: # (The workload consists of 4 types of entities:)
To recap, the key entites are:
+ [Distribution](./src/contention/benchmark/workload/distributions/abstractions/Distribution.java) — a distribution of a random variable
+ [DataMap](./src/contention/benchmark/workload/data/map/abstractions/DataMap.java) — converts a distribution's output into a key
+ [ArgsGenerator](./src/contention/benchmark/workload/args/generators/abstractions/ArgsGenerator.java) — creates operands for an operation
+ [ThreadLoop](./src/contention/benchmark/workload/thread/loops/abstractions/ThreadLoop.java) — the logic for interacting with a data structure.


<a id="bench_uml">![bench_uml.png](./bench_uml.png)</a>


There are builders each type of entity:
[ThreadLoopBuilder](./src/contention/benchmark/workload/thread/loops/abstractions/ThreadLoopBuilder.java),
[ArgsGeneratorBuilder](./src/contention/benchmark/workload/args/generators/abstractions/ArgsGeneratorBuilder.java),
[DistributionBuilder](./src/contention/benchmark/workload/distributions/abstractions/DistributionBuilder.java),
[DataMapBuilder](./src/contention/benchmark/workload/data/map/abstractions/DataMapBuilder.java).

There is also a [StopCondition](./src/contention/benchmark/workload/stop/condition/StopCondition.java)
– a condition in which the load stops working.

[//]: # (It will be described later.)

## Entities

### DataMap

The [DataMap](./src/contention/benchmark/workload/data/map/abstractions/DataMap.java) is used by an ArgsGenerator
to translate an index into a key or a value. The `get` function takes an `index` and returns the corresponding key or value.

##### NOTES
The [DataMapBuilder](./src/contention/benchmark/workload/data/map/abstractions/DataMapBuilder.java) provides the `getOrBuild` function.
If it is the first call, the function creates the new DataMap object and returns a pointer to that,
else it returns the pointer to last created object. Thus, different ArgsGenerators can work with one DataMap.  
The `getOrBuild` function does not need to be overridden. 


Also, the json representation of DataMapBuilder has object id, 
so the `getOrBuild` of the DataMapBuilder called with the same id will return a pointer to the same object.
The id is set automatically.

of the DataMapBuilder called with the same id will return a pointer to the same object.

[//]: # (This function creates the new object if)

### Distribution

The [Distribution](./src/contention/benchmark/workload/distributions/abstractions/Distribution.java) simulates some random variable.

It is important to note that it generates some value from a distribution 
that later is translated into an appropriate key or value by an ArgsGenerator. 

A Distribution's `next()` method generates a value within some range.  
We also provide a MutableDistribution, which can change the random variable in run-time by modifying the range of keys:
`setRange(int range)` and `next(int range)`, the last one run the `setRange` and `next` functions.

### ArgsGenerator

The [ArgsGenerator](./src/contention/benchmark/workload/args/generators/abstractions/ArgsGenerator.java) is used to generate keys for operations.  
In addition to specializing to the types used by a data structure, an ArgsGenerator can be stateful.  
This is important when modeling temporal locality: 
a thread-private ArgsGenerator can use a distribution 
to occasionally select recently accessed keys without extra synchronization.

##### NOTE
If your implementation of ArgsGenerator does not support some operations, 
just throw the `UnsupportedOperationException`. 

__Example:__
```java
    @Override
    public int nextGet() {
        throw new UnsupportedOperationException();
    }
```

### ThreadLoop

The [ThreadLoop](./src/contention/benchmark/workload/thread/loops/abstractions/ThreadLoop.java) decides which operation a thread should execute next. 
It is initialized for each thread separately and uses the described ArgsGenerators. 
Threads are not required to use the same ThreadLoop implementation. 
ThreadLoop has only one main method `step` 
that explains how to choose an operation and perform it during the running phase.
The method `step` is called in a conditional loop by the `run` method.
A programmer can override `run` when more complicated logic is needed.

Note that ThreadLoop is also a utility class: it calculates different statistics, 
for example, the number of successful operations. 
This is important for the benchmark in order to compare different data structures and check their correctness.


### StopCondition

The [StopCondition](./src/contention/benchmark/workload/stop/condition/StopCondition.java) determines the workload operating time.
The `isStopped` method is called by ThreadLoop before each execution of the `step` method, 
and returns `false` if the ThreadLoop should continue and `true` if the ThreadLoop should terminate.  
The countdown begins after calling the `start` function. 




