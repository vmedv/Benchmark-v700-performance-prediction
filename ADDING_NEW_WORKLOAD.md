[//]: # (# Implementing a new Workload)

[//]: # (## Software Design)

[//]: # (In [Figure]&#40;#bench_uml&#41; we manage the complexity of our more flexible benchmark through a top-down design. )

[//]: # (Each thread &#40;gray box&#41; is assigned its own ThreadLoop. )

[//]: # (Each ThreadLoop, in turn, is assigned a set of configurations,)

[//]: # (which correspond to the operations it will run &#40;light blue box&#41;.)

[//]: # (Each operation generates its arguments via a set of PRNGs, distributions over those PRNGs,)

[//]: # (and mapping functions for converting the output of a distribution into a key or value. )

[//]: # (Note that for simplicity, we depict a tree, but it is possible for a ThreadLoop to share a PRNG, DataMap,)

[//]: # (or distribution among its blue boxes, and even for a read-only DataMap to be shared among ThreadLoops.)

[//]: # ()
[//]: # ([//]: # &#40;The workload consists of 4 types of entities:&#41;)
[//]: # (To recap, the key entites are:)

[//]: # (+ [Distribution]&#40;./microbench/workloads/distributions/distribution.h&#41; — a distribution of a random variable)

[//]: # (+ [DataMap]&#40;./microbench/workloads/data_maps/data_map.h&#41; — for converting a distribution's output into a key)

[//]: # (+ [ArgsGenerator]&#40;./microbench/workloads/args_generators/args_generator.h&#41; — creates operands for an operation)

[//]: # (+ [ThreadLoop]&#40;./microbench/workloads/thread_loops/thread_loop.h&#41; — the logic for interacting with a data structure.)

[//]: # ()
[//]: # ()
[//]: # (<a id="bench_uml">![bench_uml.pdf]&#40;bench_uml.pdf&#41;</a>)

[//]: # ()
[//]: # ()
[//]: # (There are builders each type of entity:)

[//]: # ([ThreadLoopBuilder]&#40;./microbench/workloads/thread_loops/thread_loop_builder.h&#41;,)

[//]: # ([ArgsGeneratorBuilder]&#40;./microbench/workloads/args_generators/args_generator_builder.h&#41;,)

[//]: # ([DistributionBuilder]&#40;./microbench/workloads/distributions/distribution_builder.h&#41;,)

[//]: # ([DataMapBuilder]&#40;./microbench/workloads/data_maps/data_map_builder.h&#41;.)

[//]: # ()
[//]: # (There is also a [StopCondition]&#40;./microbench/workloads/stop_condition/stop_condition.h&#41;)

[//]: # (– a condition in which the load stops working.)

[//]: # ()
[//]: # ([//]: # &#40;It will be described later.&#41;)
[//]: # ()
[//]: # (### DataMap )

[//]: # ()
[//]: # (The [DataMap]&#40;./microbench/workloads/data_maps/data_map.h&#41; is used by an ArgsGenerator)

[//]: # (to translate an index into a key or value. The `get` function take an `index` and return the corresponding key or value. )

[//]: # ()
[//]: # (*NOTE:* The [DataMapBuilder]&#40;./microbench/workloads/data_maps/data_map_builder.h&#41; exists the `getOrBuild` function.)

[//]: # (If it is the first calling, the function creates the new DataMap object and returns that,)

[//]: # (else it returns the last created object. Thus, different ArgsGenerators can work with one DataMap. )

[//]: # ()
[//]: # (Also, the json representation of DataMapBuilder exist object id)

[//]: # ()
[//]: # (This function creates the new object if  )

[//]: # ()
[//]: # (### Distribution )

[//]: # ()
[//]: # ([Distribution]&#40;./microbench/workloads/distributions/distribution.h&#41;)

# The adding entity

The software design is described in [SOFTWARE_DESIGN](SOFTWARE_DESIGN.md).

The workloads are in the folder [./microbench/workloads/](./microbench/workloads/).  
Each entity's folder has the following structure:
```shell
./microbench/workloads/<entity>/
├── builders
├── impls
├── <entity>.h
├── <entity>_builder.h
└── <entity>_json_convector.h
```

The `<entity>.h` file contains the interface that needs to be implemented, 
`<entity>_builder.h` — the interface of entity builder.
The `builders` and `impls` folders contain the corresponding implementations.  
The `<entity>_json_convector.h` file is the file is an auxiliary json convector. 

### The interface implementation
The first part is the implementation of Entity:
```c++
class ExampleEntity : public Entity {
    *EntityFields*

public:
    ExampleEntity(*entityParameters*) {...}
    
    *overriding entity functions*
  
    ~ExampleEntity() {...}
};
```

### The implementation of builder
The second is the implementation of EntityBuilder:
```c++
class ExampleEntityBuilder : public EntityBuilder {
    *rawParameters*
    *finalParameters*

public:
    ExampleEntityBuilder() {...}
    
    *overriding entity functions*
  
    ExampleEntityBuilder *init(*initParameters*) override {
        *initializing final parameters by processing raw parameters*
    }

    /**
     * called only after the init function
     */
    ExampleEntity *build(Random &_rng) override {
        *creating the new instance of ExampleEntity*
    }
    
    void toJson(nlohmann::json &j) const override {
        // necessary for converting from json file to class
        j["entityType"] = EntityType::EXAMPLE_ENTITY;
        
        *converting raw parameters to json format*
    }
    
    void fromJson(const nlohmann::json &j) override {
        *converting raw parameters from json format*
    }
    
    std::string toString(size_t indents) override {
        *converting the class to string format for writing to the console*
    }
    
    ~ExampleEntityBuilder() {...}
};
```

#### Tools 

To convert to json format, the [nlohmann::json](https://github.com/nlohmann/json) implementation is used.

To convert the entities from json format use the `getEntityFromJson` function.


For convenient conversion to a string representation, use the  
`indented_title`, `indented_title_with_data` and `indented_title_with_str_data` functions
from [globals_extern.h](microbench/globals_extern.h). 


### The Adding the new entity builder to enum and json_convector 

The last part is adding the new Entity to the `EntityType` enum in the `<entity>_builder.h` file:
```c++
enum class EntityType {
..., EXAMPLE_ENTITY
};
```
And expand `getEntityFromJson` function in the `<entity>_json_convector.h` file:
```c++
EntityBuilder *getEntityFromJson(const nlohmann::json &j) {
    EntityType type = j["entityType"];
    EntityBuilder * entityBuilder;
    switch (type) {
        case EntityType::EXAMPLE_ENTITY:
            entityBuilder = new ExampleEntityBuilder();
            break;
        case {...}
    }    
    entityBuilder->fromJson(j);
    return entityBuilder;
}
```

### StopCondition

In contrast to other entities, `StopCondition` does not have builders, so it is converted to json format on its own
and initialization occurs during the call to the `start(numThreads)` function.

## Example

### DataMap 

The `ArrayDataMap` creates an array filled with values from the entire range of keys and shuffles them randomly. 
When calling the `get(index)` method, returns the corresponding element from the array.  

The [implementation](./microbench/workloads/data_maps/impls/array_data_map.h) 
and [builder](./microbench/workloads/data_maps/builders/array_data_map_builder.h) of `ArrayDataMap`.

### Distribution

The `SkewedUniformDistribution` depends on two variables `hotSize` and `hotProb`. 
The range is divided into two interval: 
the random variable returns from first interval with `hotProb%` probability and from second with `100 - hotProb%`;
the size of first interval is `range * hotSize`, the size of second if `range * (1 - hotSize). 
The random variable in intervals chooses uniformly. 

The [implementation](./microbench/workloads/distributions/impls/skewed_uniform_distribution.h)
and [builder](./microbench/workloads/distributions/builders/skewed_uniform_distribution_builder.h) of `SkewedUniformDistribution`.

### ArgsGenerator

The `SkewedSetsArgsGenerator` uses two `SkewedUniformDistributions` separately for read and update operations,
and takes the following parameters:
+ `rp%` of read operations are performed on a random subset of keys of proportion `rs%` where a key is taken uniformly. 
All other read operations are performed on the rest of the set.
+ `wp%` of update operations are performed on a random subset of keys of proportion `ws%` where a key is taken uniformly.
All other update operations are performed on the rest of the set.
+ `inter%` of keys are in the intersection of the working sets of read and update operations.

The implementation and builder are in one [file](./microbench/workloads/args_generators/impls/skewed_sets_args_generator.h)
of `SkewedSetsArgsGenerator`.

### TreadLoop

The `DefaultThreadLoop` selects the next operation with some fixed probability. It accepts the following parameters:
+ `ui%` of operations are insert operations;
+ `ue%` of operations are remove operations;
+ while `100 - ui - ue%` of operations are get operations.

The implementation and builder are in one [file](./microbench/workloads/thread_loops/impls/default_thread_loop.h)
of `DefaultThreadLoop`.

### StopCondition

The `Timer` accepts a `workTime` parameter in milliseconds, and the `isStopped` method returns true during that time.

The implementation is in one [file](./microbench/workloads/stop_condition/impls/timer.h) of `Timer`.



[//]: # (## Example)
[//]: # ()
[//]: # (We now present a complete example, by adding the skewed read-update workload from)
[//]: # ([aksenov2023splay]&#40;https://link.springer.com/article/10.1007/s00446-022-00441-x&#41; in our suite.)
[//]: # (This workload is used for testing key-value data structures supporting three operations `insert`, `remove`, and `get`.)
[//]: # ()
[//]: # (This workload is specified by five parameters:)
[//]: # (+ `n`, the size of the working set of keys;)
[//]: # (+ `w%`, the amount of updates in the total number of operations;)
[//]: # (+ `x%` of get operations choose a key uniformly at random from a random subset of keys of proportion `y%`, )
[//]: # (while other get operations choose a random key from the rest of the set;)
[//]: # (+ `insert` and `remove` operations choose a key uniformly at random from a random subset of keys of proportion `s%`.)




