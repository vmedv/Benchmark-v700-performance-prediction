#!/bin/bash

if [ "$#" -ne "0" ]; then
    testing="testing"
else
    testing=""
fi

outdir=output
./_run_experiments.sh $testing \
    && ./_create_csv.sh \
    && ./_create_graphs.sh \
    && cp ../../tools/gen_html/* ${outdir}/ \
    && cd ${outdir} \
    && ./_gen.sh "Simple experiment" "A worked example"
