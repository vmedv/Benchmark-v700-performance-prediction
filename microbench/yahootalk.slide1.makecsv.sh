#!/bin/bash
# 
# File:   yahootalk.slide1.makecsv.sh
# Author: trbot
#
# Created on Sep 4, 2017, 9:28:39 PM
#

outdir=data

for fname in `ls $outdir/step*.out` ; do
    reclaim=`cat $fname | head -1 | tr " " "\n" | grep "reclaim" | tr "." "\n" | grep "reclaim" | cut -d"_" -f2`
    pool=`cat $fname | head -1 | tr " " "\n" | grep "reclaim" | tr "." "\n" | grep "pool"`
    alg=`cat $fname | head -1 | tr " " "\n" | grep "reclaim" | tr "." "\n" | head -2 | tail -1`
    prefetching=`cat $fname | head -1 | tr " " "\n" | grep "0x[0-9]$"`
    binding=`cat $fname | head -1 | tr " " "\n" | grep "," | tr "," "_"`
    alloc=`cat $fname | head -1 | tr " " "\n" | grep "TREE_MALLOC"`
    throughput=`cat $fname | grep "total throughput" | cut -d":" -f2 | tr -d " "`
    echo $alg,$alloc,$reclaim,$pool,$prefetching,$binding,$throughput
done
