# SETUP

First of all, it's necessary to clone the repository with submodules:

```shell
git clone https://gitlab.com/mr_ravil/setbench.git --recurse-submodules
git checkout json
```

The project has the following structure:
```shell
.
├── common
├── docker
├── ds
├── json
├── lib
├── macrobench
├── macrobench_experiments
├── microbench
├── microbench_experiments
└── tools
```

The main folders to pay attention to are [ds](./ds) and [microbench](./microbench/). The first one stands for data structures - all available for benchmarking data structures are stored there. The latter stands for launching benchmarks and there you can specify many arguments, in particular, [different workloads](./WORKLOADS.md).

Before launching benchmarks for different data structures --- it's necessary to build the project and it can be done with the following command:

```shell
cd setbench/microbench
make -j
```


# LAUNCH

After setting up the project, you can launch benchmarks. Commands to launch have the following pattern:

```shell
cd setbench/microbench

LD_PRELOAD=../lib/libjemalloc.so ./bin/<data_structure_name>.debra <bench_args>
```

Example:
```shell
LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra -json-file json_example.txt -result-file result_example.txt 
```

## Benchmark arguments

+ `-json-file <file_name>` — file with launch parameters in the json format ([BenchParameters](./microbench/workloads/bench_parameters.h), [example](microbench/json_example.cpp))
+ `-result-file <file_name>` — file to output the results in the json format (optional)

## Troubleshooting

If something breaks after the launch, or there is such a problem:

```shell
PAPI ERROR: thread 0 unable to add event PAPI_L2_DCM: Permission level does not permit operation
```
then the following can help:

```shell
sudo sysctl kernel.perf_event_paranoid=1
```

# Configuring Launch Parameters

[Example of configuring launch parameters](./microbench/json_example/json_example.cpp).
Use [Makefile](./microbench/json_example/Makefile) to compile.

The first step is the creation the [BenchParameters class](./microbench/workloads/bench_parameters.h).

```c++
    BenchParameters benchParameters;
```

Set the range of keys.

```c++
    benchParameters.setRange(2048);
```

Create the [Parameters class](./microbench/workloads/parameters.h) for benchmarking (test).

```c++
    Parameters test;
```

We will need to set the [stop condition](./microbench/workloads/stop_condition/stop_condition.h) and workloads.

First, let's create and set a stop condition: [Timer](./microbench/workloads/stop_condition/impls/timer.h) with 10 second (10000 millis).

```c++
    StopCondition * stopCondition = new Timer(10000);
    test.setStopCondition(stopCondition);
```

The next step is the setup a workload.

The workload consists of 4 types of entities:
+ Distribution — a distribution of a random variable
+ DataMap — for converting a distribution's output into a key
+ ArgsGenerator — creates operands for an operation
+ ThreadLoop — the logic for interacting with a data structure.

There are builders to create each type of entity:
[ThreadLoopBuilder](./microbench/workloads/thread_loops/thread_loop_builder.h),
[ArgsGeneratorBuilder](./microbench/workloads/args_generators/args_generator_builder.h),
[DistributionBuilder](./microbench/workloads/distributions/distribution_builder.h),
[DataMapBuilder](./microbench/workloads/data_maps/data_map_builder.h).

Let's create a standard workload with Zipf distribution.

At first create the DistributionBuilder and DataMapBuilder.
```c++
    DistributionBuilder * distributionBuilder 
            = (new ZipfDistributionBuilder())
                    ->setAlpha(1.0);

    DataMapBuilder * dataMapBuilder 
            = new ArrayDataMapBuilder();
```

The next step is to create the ArgsGeneratorBuilder.
```c++
    ArgsGeneratorBuilder *argsGeneratorBuilder
            = (new DefaultArgsGeneratorBuilder())
                    ->setDistributionBuilder(distributionBuilder)
                    ->setDataMapBuilder(dataMapBuilder);
```

The last step is to create the ThreadLoopBuilder. 
We create a DefaultThreadLoop with the probability 0.1 (10%) of calling the insertion and remove operation
and set our ArgsGeneratorBuilder
```c++
    ThreadLoopBuilder *threadLoopBuilder
            = (new DefaultThreadLoopBuilder())
                    ->setInsRatio(0.1)
                    ->setRemRatio(0.1)
                    ->setArgsGeneratorBuilder(argsGeneratorBuilder);
```

Now set the ThreadLoop class to Parameters with the number of threads with this load
```c++
    test.addThreadLoopBuilder(*threadLoopBuilder, 8);
```

Specify the test parameters in the benchParameters and create a default prefill (fill the data structure in half):
```c++
    benchParameters.setTest(test).createDefaultPrefill();
```

Convert parameters to json format and output:
```c++
    nlohmann::json json = benchParameters;

    std::ofstream out("json_example.txt");

    out << json.dump(4);
```
