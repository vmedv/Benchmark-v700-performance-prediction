#!/bin/bash
# 
# File:   runscript.sh
# Author: trbot
#
# Created on Jul 18, 2017, 10:56:28 PM
#

ntrials=5

outpath=data
fsummary=$outpath/summary.txt

mv $outpath $outpath.old
mkdir -p $outpath

nthreads=36

cnt1=10000
cnt2=10000

for counting in 1 0 ; do
    if (($counting==0)); then
        echo "Total trials: $cnt1 ... $cnt2"
    fi
    
    for ((trial=0; trial < $ntrials; ++trial)); do
        for exepath in `ls ./bin/rundb*`; do
            if (($counting==1)); then
                cnt2=`expr $cnt2 + 1`
                if ((($cnt2 % 100) == 0)); then echo "Counting trials: $cnt2" ; fi
            else
                cnt1=`expr $cnt1 + 1`
                exeonly=`echo $exepath | cut -d"/" -f3`
                fname=$outpath/step$cnt1.trial$trial.$exeonly.txt
                workload=`echo $exeonly | cut -d"_" -f2`
                datastructure=`echo $exeonly | cut -d"_" -f3- | cut -d"." -f1`

                #echo "RUNNING step $cnt1 / $cnt2 : trial $trial of $exeonly > $fname"
                
                echo -n "step=$cnt1, trial=$trial, workload=$workload, datastructure=$datastructure," >> $fsummary

                args="-t$nthreads -n$nthreads"
                cmd="env LD_PRELOAD=../lib/libjemalloc-5.0.1-25.so $exepath $args"
                echo $fname > $fname
                echo $cmd >> $fname
                $cmd >> $fname
                cat $fname | grep "summary" | cut -d"]" -f2- >> $fsummary
                tail -1 $fsummary
            fi
        done
    done
done

#./makecsv.sh
