#!/bin/bash

#########################################################################
#### Experiment configuration
#########################################################################

exp="disable_multicounter"
t=30000
num_trials=3
timeout_s=600
halved_update_rates="0.5 5 20"
key_range_sizes="2000000 20000000 200000000"
algorithms="brown_ext_ist_lf"
thread_counts=`cd .. ; ./get_thread_count_max.sh`
compile_args_for_disabled="-DIST_DISABLE_MULTICOUNTER_AT_ROOT -DMEASURE_REBUILDING_TIME"
compile_args_for_enabled="-DMEASURE_REBUILDING_TIME"

echo "#########################################################################"
echo "#### Compiling binaries with the desired functionality disabled"
echo "#########################################################################"

here=`pwd`
mkdir ${here}/bin 2>/dev/null
cd .. ; cd .. ; make -j all bin_dir=${here}/bin_disabled xargs="$compile_args_for_disabled" > compiling.txt 2>&1
if [ "$?" -ne "0" ]; then
    echo "ERROR compiling; see compiling.txt"
    exit
fi
cd $here

echo "#########################################################################"
echo "#### Compiling binaries with the desired functionality enabled"
echo "#########################################################################"

here=`pwd`
mkdir ${here}/bin 2>/dev/null
cd .. ; cd .. ; make -j all bin_dir=${here}/bin_enabled xargs="$compile_args_for_enabled" >> compiling.txt 2>&1
if [ "$?" -ne "0" ]; then
    echo "ERROR compiling; see compiling.txt"
    exit
fi
cd $here

#########################################################################
#### Produce header
#########################################################################

mkdir $exp 2>/dev/null
../parse.sh null > $exp.csv
cat $exp.csv

step=10000
maxstep=$step
pinning_policy=`cd .. ; ./get_pinning_cluster.sh`

#########################################################################
#### Run trials with the desired functionality disabled, then enabled
#########################################################################

started=`date`
for counting in 1 0 ; do
    for mode in disabled enabled ; do
        for ((trial=0;trial<num_trials;++trial)) ; do
            for uhalf in $halved_update_rates ; do
                for k in $key_range_sizes ; do
                    for alg in $algorithms ; do
                        for n in $thread_counts ; do
                            if ((counting)); then
                                maxstep=$((maxstep+1))
                            else
                                step=$((step+1))
                                if [ "$#" -eq "1" ]; then ## check if user wants to just replay one precise trial
                                    if [ "$1" -ne "$step" ]; then
                                        continue
                                    fi
                                fi

                                f="$exp/step$step.txt"
                                args="-nwork $n -nprefill $n -i $uhalf -d $uhalf -rq 0 -rqsize 1 -k $k -nrq 0 -t $t -pin $pinning_policy"
                                cmd="LD_PRELOAD=../../../lib/libjemalloc.so timeout $timeout_s numactl --interleave=all time bin_${mode}/ubench_${alg}.alloc_new.reclaim_debra.pool_none.out $args"
                                echo "cmd=$cmd" > $f
                                echo "step=$step" >> $f
                                echo "fname=$f" >> $f
                                eval $cmd >> $f 2>&1

                                ## manually parse the maximum resident size from the output of `time` and add it to the step file
                                maxres=`../grep_maxres.sh $f`
                                echo "maxresident_mb=$maxres" >> $f

                                ## parse step file to extract fields of interest, and also *rename* the algorithm to explicitly encode whether the functionality is enabled or disabled
                                ../parse.sh $f | tail -1 | sed 's/${alg}/${alg}_${mode}/g' >> $exp.csv

                                echo -n "step $step/$maxstep: "
                                cat $exp.csv | tail -1
                            fi
                        done
                    done
                 done
            done
        done
    done
done

echo "started: $started"
echo "finished:" `date`
