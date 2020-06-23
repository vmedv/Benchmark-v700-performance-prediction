#!/bin/bash

#########################################################################
#### Experiment configuration
#########################################################################

millis_to_run=30000
num_trials=1
insert_delete_fractions=( "0 0" "0.5 0.5" "9 1" )
key_range_sizes=( 2000000 20000000 )
algorithms=( brown_ext_ist_lf brown_ext_abtree_lf bronson_pext_bst_occ )

## get some sensible thread counts for this system automatically
thread_counts=( $(cd ../../tools ; ./get_thread_counts_numa_nodes.sh) )
## could specify manually as follows: thread_counts="6 12 18 24 30 36 54 72"

## get a sensible thread pinning policy for this system automatically
pinning_policy=$(cd ../../tools ; ./get_pinning_cluster.sh)
## could specify manually as follows: pinning_policy="0-23,96-119,24-47,120-143,48-71,144-167,72-95,168-191"

prefill=1
other_args="-rq 0 -rqsize 1 -nrq 0"

timeout_sec=300  ## how long to allow a trial to run (in seconds) before killing it (to deal with infinite loops, prefilling that is too slow, etc.)

host=$(hostname)
curr_dir=$(pwd)
bin_dir=${curr_dir}/${host}_bin
data_dir=${curr_dir}/${host}_data
flog=${data_dir}/${host}_log.txt
ferr=${data_dir}/${host}_err.txt
compile_dir="../../microbench"
lib_dir="../../lib"

malloc_lib="${lib_dir}/libjemalloc.so"

#########################################################################
#### Backup ONE set of old data and binaries (to avoid mistakes...)
#########################################################################

echo
echo "## Moving old binaries and data to *.old"
echo

rm -rf $bin_dir.old >/dev/null 2>&1
rm -rf $data_dir.old >/dev/null 2>&1
mv $bin_dir $bin_dir.old >/dev/null 2>&1
mv $data_dir $data_dir.old >/dev/null 2>&1
mkdir $bin_dir
mkdir $data_dir

#########################################################################
#### Check whether we should run in TESTING mode (if there's any argument to this script)
#########################################################################

if [ "$#" -ne "0" ]; then
    echo "WARNING: TESTING mode enabled" | tee $flog
    num_trials=1
    millis_to_run=100
    thread_counts=( "1" $(cd ../../tools ; ./get_thread_counts_max.sh) )
    prefill=0
else
    echo "Note: PRODUCTION mode enabled" | tee $flog
fi

#########################################################################
#### Log script parameters variables
#########################################################################

echo | tee -a $flog
echo "## Script running with the following parameters:" | tee -a $flog
echo "   note: logging to file $flog" | tee -a $flog
echo | tee -a $flog

echo millis_to_run=$millis_to_run | tee -a $flog
echo num_trials=$num_trials | tee -a $flog
echo insert_delete_fractions=${insert_delete_fractions[@]} | tee -a $flog
echo key_range_sizes=${key_range_sizes[@]} | tee -a $flog
echo algorithms=${algorithms[@]} | tee -a $flog
echo thread_counts=${thread_counts[@]} | tee -a $flog
echo pinning_policy=${pinning_policy[@]} | tee -a $flog
echo timeout_sec=$timeout_sec | tee -a $flog
echo host=$host | tee -a $flog
echo curr_dir=$curr_dir | tee -a $flog
echo bin_dir=$bin_dir | tee -a $flog
echo data_dir=$data_dir | tee -a $flog
echo compile_dir=$compile_dir | tee -a $flog
echo lib_dir=$lib_dir | tee -a $flog
echo malloc_lib=$malloc_lib | tee -a $flog
echo | tee -a $flog

#########################################################################
#### Compile into bin_dir
#########################################################################

echo | tee -a $flog
echo "## Compiling into ${bin_dir}" | tee -a $flog
echo | tee -a $flog

## compile in a subshell to avoid changing directory...
bash -c "cd ${compile_dir} ; make -j all bin_dir=${bin_dir} 2>&1 >>$flog"
if [ "$?" -ne "0" ]; then
    echo "ERROR: compiling... see $flog" | tee -a $flog
    exit 1
else
    echo "Success." | tee -a $flog
fi

#########################################################################
#### Run trials
#########################################################################

started=`date`
echo | tee -a $flog
echo "## Running trials (starting at $started)" | tee -a $flog
echo | tee -a $flog

step=0
maxstep=$(echo "$num_trials * ${#insert_delete_fractions[@]} * ${#key_range_sizes[@]} * ${#algorithms[@]} * ${#thread_counts[@]}" | bc)

# echo maxstep=$maxstep
# echo "multipliers:"
# echo $num_trials
# echo ${#insert_delete_fractions[@]}
# echo ${#key_range_sizes[@]}
# echo ${#algorithms[@]}
# echo ${#thread_counts[@]}

for ((trial=0; trial<num_trials; ++trial)) ; do
    for ins_del_frac in "${insert_delete_fractions[@]}" ; do
        ins_frac=$(echo $ins_del_frac | cut -d" " -f1)
        del_frac=$(echo $ins_del_frac | cut -d" " -f2)

        for k in "${key_range_sizes[@]}" ; do
            for alg in "${algorithms[@]}" ; do
                for n in "${thread_counts[@]}" ; do

                    ## actually run current trial
                    step=$((step+1))
                    f=$(printf "%s%06d%s.txt" "${data_dir}/data" "$step" "_i${ins_frac}d${del_frac}_k${k}_${alg}_n${n}")
                    echo "$step / $maxstep: $f" | tee -a $flog

                    prefill_str="" ; if [ "$prefill" -eq "1" ]; then prefill_str="-nprefill $n" ; fi
                    args="-nwork $n $prefill_str -i $ins_frac -d $del_frac -k $k -t $millis_to_run -pin $pinning_policy $other_args"
                    cmd="LD_PRELOAD=${malloc_lib} timeout $timeout_sec numactl --interleave=all time ${bin_dir}/ubench_${alg}.alloc_new.reclaim_debra.pool_none.out $args"

                    echo "cmd=$cmd" > $f
                    echo "fname=$f" >> $f
                    echo "step=$step" >> $f

                    eval $cmd >> $f 2>&1
                    if [ "$?" -ne "0" ]; then
                        echo "ERROR in run $f" >> $ferr
                    fi

                    ## manually parse the maximum resident size from the output of `time` and add it to the data file
                    maxres=$(echo $(cat $f | grep "maxres" | cut -d" " -f6 | cut -d"m" -f1) / 1000 | bc)
                    echo "maxresident_mb=$maxres" >> $f

                done
            done
        done
    done
done

echo | tee -a $flog
echo "started: $started" | tee -a $flog
echo "finished: $(date)" | tee -a $flog

echo | tee -a $flog
echo "## Zipping *.sh and ${data_dir} into ./${host}_data.zip" | tee -a $flog
echo | tee -a $flog
zip -r ./${host}_data.zip *.sh ${data_dir}

echo | tee -a $flog
echo "Log can be viewed at $flog"

if [ -e "$ferr" ] && [ "0$(cat $ferr | wc -l)" -ne "0" ]; then
    echo
    echo "#########################################################################"
    echo "#### ERROR: see $ferr"
    echo "#########################################################################"
    echo
fi
