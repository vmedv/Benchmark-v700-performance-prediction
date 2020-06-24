#!/bin/bash

if [ "$#" -ne "0" ]; then
    testing="testing"
else
    testing=""
fi

outdir=output
flog=${outdir}/_log_do_all.txt
./_run_experiments.sh $testing | tee $flog \
    && ./_create_csv.sh | tee -a $flog \
    && ./_create_plots.sh | tee -a $flog \
    && cp ../../tools/gen_html/* ${outdir}/ 2>&1 | tee -a $flog \
    && cd ${outdir} | tee -a $flog \
    && ./_gen.sh "A simple experiment" "With rather sophisticated automated tooling!" | tee -a $flog

####################################################################
#### TODO
####################################################################

## make page rows/cols explicit AS OPTIONAL ARGUMENTS IN GEN?

## proper throughput line plot options (mostly marker customization?)
## possibly extract something generic from this latest test
## test this tooling on a system/LOGIN WITHOUT global "tools" access (would this be true for CI?)

## maxres plot with baseline adjustment?

## clean up compile.sh
## copy direct clone into into README.md

## advplot into tools
## better timeline capture

## equivalent of this for macrobench?

## stress test scripts
## leak checking scripts
## flamegr script
## dashboard for performance comparison of DS

## experiment example for comparison of reclamation algs
## memhook integration with example experiments
## worked example of DS perf comparison understanding from basic cycles -> instr/cachemiss -> location w/perfrecord -> memhook layout causation
##      temporal profiling with perf -> temporal flamegr of measured phase
