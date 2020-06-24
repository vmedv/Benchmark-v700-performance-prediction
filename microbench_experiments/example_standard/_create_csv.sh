#!/bin/bash

fields+=( step )
fields+=( hostname )
fields+=( DS_TYPENAME )
fields+=( TOTAL_THREADS )
fields+=( RQ_THREADS )
fields+=( MAXKEY )
fields+=( INS )
fields+=( DEL )
fields+=( total_throughput )
fields+=( PAPI_L2_TCM )
fields+=( PAPI_L3_TCM )
fields+=( PAPI_TOT_CYC )
fields+=( PAPI_TOT_INS )

echo "num_steps=$(echo *_data/data*.txt | wc -w)"
echo "${fields[@]}" | tee data.csv
for f in *_data/data*.txt ; do
    cat $f | ../../tools/fields.sh $f "${fields[@]}" | tee -a data.csv
done

## separate fields array into decorative, filtering and values
## make fields array an include? so it applies identically to this script and _create_graphs.sh?
## next step... create graphs?

## create unifying script that calls all relevant _*.sh scripts
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
