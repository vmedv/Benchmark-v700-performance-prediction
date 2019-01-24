#!/bin/bash

exp="data"
rm $exp.csv 2>/dev/null
mkdir $exp 2>/dev/null
echo "" > $exp.csv

step=10000
maxstep=$step
thread_counts=`cd .. ; ./get_thread_counts.sh`
pinning_policy=`cd .. ; ./get_pinning_cluster.sh`
t=16000

for counting in 1 0 ; do
    for ((trial=0;trial<5;++trial)) ; do
        for uhalf in 0 0.5 5 20 ; do
            for k in 2000000 20000000 200000000 ; do
                for alg in brown_ext_ist_lf brown_ext_abtree_lf bronson_pext_bst_occ ellen_ext_bst_lf guerraoui_ext_bst_ticket natarajan_ext_bst_lf ; do
                    for n in $thread_counts ; do
                        if ((counting)); then
                            maxstep=$((maxstep+1))
                        else
                            step=$((step+1))
                            args="-nwork $n -nprefill $n -i $uhalf -d $uhalf -rq 0 -rqsize 1 -k $k -nrq 0 -t $t -pin $pinning_policy"
                            LD_PRELOAD=../../../lib/libjemalloc-5.0.1-25.so numactl --interleave=all ../../bin/${alg}.ubench_rdebra $args > $exp/step$step.txt
                            ../parse.sh $exp/step$step.txt | tail -1 >> $exp.csv
                            echo -n "step $step/$maxstep: "
                            cat $exp.csv | tail -1
                        fi
                    done
                done
             done
        done
    done
done
