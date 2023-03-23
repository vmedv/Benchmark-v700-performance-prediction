# LAUNCH

After setting up the project, you can launch benchmarks. Commands to launch have the following pattern:

```shell
cd setbench/microbench

LD_PRELOAD=../lib/libjemalloc.so ./bin/<data_structure_name>.debra <workload_type> <parameters>
```

Example:
```shell
LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra -skewed-sets 
    -rp 0.9 -rs 0.1 -wp 0.9 -ws 0.2 -inter 0.05 -i 0.05 -d 0.05 -rq 0 -k 100000 
    -prefillsize 100000 -nprefill 8 -t 10000 -nrq 0 -nwork 8 -prefill-insert 
```

## Troubleshooting

If something breaks after the launch, or there is such a problem:

```shell
PAPI ERROR: thread 0 unable to add event PAPI_L2_DCM: Permission level does not permit operation
```
then the following can help:

```shell
sudo sysctl kernel.perf_event_paranoid=1
```
