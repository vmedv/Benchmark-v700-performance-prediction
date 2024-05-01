# The adding entity

The software design is described in [SOFTWARE_DESIGN](SOFTWARE_DESIGN.md).

The workloads are in the folder [./src/contention/benchmark/workload/](./src/contention/benchmark/workload/).  
Each entity's folder has the following structure:
```shell
./src/contention/benchmark/workload/<entity>/
├── abstracrion
├── builders
└── impls
```

The `<entity>.java` file contains the interface that needs to be implemented, 
`<entity>_builder.java` — the interface of entity builder.
The `builders` and `impls` folders contain the corresponding implementations.  

### The interface implementation
The first part is the implementation of an Entity:
```java
public class ExampleEntity implements Entity {
    /*EntityFields*/

    public ExampleEntity(/*entityParameters*/) {...}
    
    /*overriding entity functions*/
}
```

### The implementation of a builder
The second part is the implementation of an EntityBuilder:
```java
public class ExampleEntityBuilder implements EntityBuilder {
    /*rawParameters*/
    /*transient finalParameters*/

    public ExampleEntityBuilder() {...}
    
    /*overriding entity functions*/

    @Override
    public ExampleEntityBuilder init(/*initParameters*/) {
        /*initializing final parameters by processing raw parameters*/
    }

    /**
     * called only after the init function
     */
    @Override
    public ExampleEntity build() {
        /*creating the new instance of ExampleEntity*/
    }

    @Override
    public StringBuilder toStringBuilder(int indents) {
        /*converting the class to string format for writing to the console*/
    }

}
```

#### Tools 

To convert to json format, the [Gson](https://github.com/google/gson) implementation is used.
Conversion occurs through the [JsonConvector](src/contention/benchmark/json/JsonConverter.java) class.

Sign parameters that should not be converted to json format using the `transient` modifier.

For convenient conversion to a string representation, use the  
`indentedTitle`, `indentedTitleWithData` and `indentedTitleWithDataPercent` functions
from [StringFormat](src/contention/benchmark/tools/StringFormat.java). 


### StopCondition

In contrast to other entities, `StopCondition` does not have builders, so it is converted to json format on its own
and initialization occurs during the call to the `start(numThreads)` function.

## Example

### DataMap 

The `ArrayDataMap` creates an array filled with values from the entire range of keys and shuffles them randomly. 
When calling the `get(index)` method, returns the corresponding element from the array.  

The [implementation](src/contention/benchmark/workload/data/map/impls/ArrayDataMap.java) 
and [builder](src/contention/benchmark/workload/data/map/builders/ArrayDataMapBuilder.java) of `ArrayDataMap`.

### Distribution

The `SkewedUniformDistribution` depends on two variables `hotSize` and `hotRatio`, which take values from 0 to 1. 
The range is divided into two interval: 
the random variable returns from first interval with `hotRatio` probability and from second with `1 - hotRatio`;
the size of first interval is `range * hotSize`, the size of second if `range * (1 - hotSize)`. 
The random variable in intervals chooses uniformly. 

The [implementation](src/contention/benchmark/workload/distributions/impls/SkewedUniformDistribution.java)
and [builder](src/contention/benchmark/workload/distributions/builders/SkewedUniformDistributionBuilder.java) of `SkewedUniformDistribution`.

### ArgsGenerator

The `SkewedSetsArgsGenerator` uses two `SkewedUniformDistributions` separately for read and update operations,
and takes the following parameters:
+ `rp%` of read operations are performed on a random subset of keys of proportion `rs%` where a key is taken uniformly. 
All other read operations are performed on the rest of the set.
+ `wp%` of update operations are performed on a random subset of keys of proportion `ws%` where a key is taken uniformly.
All other update operations are performed on the rest of the set.
+ `inter%` of keys are in the intersection of the working sets of read and update operations.

The [implementation](src/contention/benchmark/workload/args/generators/impls/SkewedSetsArgsGenerator.java)
and [builder](src/contention/benchmark/workload/args/generators/builders/SkewedSetsArgsGeneratorBuilder.java) of `SkewedSetsArgsGenerator`.

### TreadLoop

The `DefaultThreadLoop` selects the next operation with some fixed probability. It accepts the following parameters:
+ `ui%` of operations are insert operations;
+ `ue%` of operations are remove operations;
+ while `100 - ui - ue%` of operations are get operations.

The [implementation](src/contention/benchmark/workload/thread/loops/impls/DefaultThreadLoop.java)
and [builder](src/contention/benchmark/workload/thread/loops/builders/DefaultThreadLoopBuilder.java) of `DefaultThreadLoop`.

### StopCondition

The `Timer` accepts a `workTime` parameter in milliseconds, and the `isStopped` method returns true during that time.

The implementation are presented in [Timer file](src/contention/benchmark/workload/stop/condition/Timer.java)
