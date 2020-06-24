#!/bin/bash

source _fields.inc

files=$(echo *_data/data*.txt)
echo "num_steps=$(echo $files | wc -w)"
eval "parallel \" ../../tools/fields.sh {} ${fields[@]} | tee {}.csv \" ::: $files"
echo "${fields[@]}" | tee data.csv
cat *_data/data*.txt.csv | sort -n >> data.csv
rm *_data/data*.txt.csv

## update _create_graphs.sh so it creates txt log files for clickthrough (from step values)
## test new gen and deploy_site tool scripts

## proper throughput line graph options (mostly marker customization?)
## possibly extract something generic from this latest test
## test this tooling on a system/LOGIN WITHOUT global "tools" access (would this be true for CI?)

## maxres graph with baseline adjustment?

## clean up compile.sh
## copy direct clone into into README.md

## advplot into tools
## better timeline capture

## equivalent of this for macrobench?

## stress test scripts
## leak checking scripts
## flame graph script
## dashboard for performance comparison of DS

## experiment example for comparison of reclamation algs
## memhook integration with example experiments
## worked example of DS perf comparison understanding from basic cycles -> instr/cachemiss -> location w/perfrecord -> memhook layout causation
##      temporal profiling with perf -> temporal flame graphs of measured phase
