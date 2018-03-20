#!/bin/bash

source ../plaf.inc

## get list of data structure microbenchmark binaries
binaries=`ls bin/[^_]*.ubench`

## generate list of experiments

fname=experiment_list.txt
rm -f $fname 2&>1 > /dev/null
#echo u,rq,rqsize,k,nrq,nwork,ds > $fname

echo "Generating..."
rq=0
rqsize=0
nrq=0
for u in 0 10 50 ; do
for k in 100000 1000000 10000000 ; do
for bin in $binaries ; do
for nwork in $thread_counts ; do
    
    ## filter out unwanted combinations of parameters
    if [ "$ds" == "*abtree*" ] || [ "$ds" == "*bslack*" ]; then
        if [ "$k" -lt "1000000" ]; then continue; fi
    fi
    if [ "$k" -gt "1000000" ]; then continue; fi

    ## emit experiment
    ds=`echo $bin | cut -d"/" -f2 | cut -d"." -f1`
    echo $u $rq $rqsize $k $nrq $nwork $ds $bin >> $fname
done
done
done
done

echo "Total experiment lines generated:" `cat $fname | wc -l`
