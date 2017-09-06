#!/bin/bash
# 
# File:   yahootalk.slide1.sh
# Author: trbot
#
# Created on Sep 4, 2017, 7:56:05 PM
#

outdir=data
mkdir $outdir > /dev/null 2>&1
fsummary=$outdir/summary.txt
trials=5

cnt1=10000
cnt2=10000

export PATH=.:$PATH

for counting in 1 0 ; do
    if (($counting==0)); then
        echo "Total trials: $cnt1 ... $cnt2"
    fi

    for pfcmd in "wrmsr~-a~0x1a4~0x7" "wrmsr~-a~0x1a4~0x0" ; do
    for alloccmd in "env~LD_PRELOAD=../lib/libjemalloc-5.0.1-25.so~TREE_MALLOC=../lib/libjemalloc-5.0.1-25.so" "env~TREE_MALLOC=" ; do
    for binding in "0-11,24-35,12-23,36-47" "0,24,12,36,1,25,13,37,2,26,14,38,3,27,15,39,4,28,16,40,5,29,17,41,6,30,18,42,7,31,19,43,8,32,20,44,9,33,21,45,10,34,22,46,11" ; do
    for alg in `ls *bronson*reclaim* *natarajan*stage2*reclaim*` ; do
    for ((trial=0;trial<$trials;++trial)) ; do
        if (($counting==1)); then
            cnt2=`expr $cnt2 + 1`
            if ((($cnt2 % 100) == 0)); then echo "Counting trials: $cnt2" ; fi
        else
            cnt1=`expr $cnt1 + 1`
            fname="$outdir/step$cnt1.out"

            acmd=`echo $alloccmd | tr "~" " "`
            pcmd=`echo $pfcmd | tr "~" " "`
            workload="-i 50 -d 50 -k 200000 -p -nwork 24"
            args="$workload -t 3000 -bind $binding"
            $pcmd
            cmd="$acmd $alg $args"
            echo "$pcmd ; $cmd >> $fname" > $fname
            echo "$fname" >> $fname
            $cmd >> $fname
            throughput=`cat $fname | grep "total throughput" | cut -d":" -f2 | tr -d " "`
            echo "step$cnt1: $throughput ($cmd)" >> $fsummary
            tail -1 $fsummary
        fi
    done
    done
    done
    done
    done
done