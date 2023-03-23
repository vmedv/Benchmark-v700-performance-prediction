# SETUP

First of all, it's necessary to clone the repository with submodules:

```shell
git clone https://gitlab.com/mr_ravil/setbench.git --recurse-submodules
git checkout setbench_self_adjusting_trees
```

The project has the following structure:
```shell
.
├── common
├── docker
├── ds
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
