#!/usr/bin/python3

import inspect
import subprocess
import os
import pprint

g = dict()
g['run_params'] = dict()
g['data_fields'] = set()
g['replacements'] = dict()
g['plots'] = []
g['log'] = open('log.txt', 'w')

pp_log = pprint.PrettyPrinter(indent=4, stream=g['log'])
pp_stdout = pprint.PrettyPrinter(indent=4)

def set_dir_compile(str):
    g['replacements']['__dir_compile'] = str

def set_dir_run(str):
    g['replacements']['__dir_run'] = str

def set_dir_data(str):
    g['replacements']['__dir_data'] = str

def set_cmd_compile(str):
    g['cmd_compile'] = str

def set_cmd_run(str):
    g['cmd_run'] = str

def set_outfile_run(str):
    g['outfile_run'] = str

def get_dir_compile():
    return g['replacements']['__dir_compile']

def get_dir_run():
    return g['replacements']['__dir_run']

def get_dir_data():
    return g['replacements']['__dir_data']

def add_run_param(name, value_list):
    # print('{}('{}', {})'.format(inspect.currentframe().f_code.co_name, name, value_list))
    g['run_params'][name] = value_list

def add_data_field(name):
    g['data_fields'].add(name)

def add_plots(filename, x_axis, y_axis, plot_type, series=None, groupby_list=[], filter="", title="", plot_cmd_args=""):
    g['plots'].append(locals())

def shell_to_str(cmd):
    return subprocess.run(cmd, stdout=subprocess.PIPE, shell=True).stdout.decode('utf-8').rstrip(' \r\n')

def shell_to_list(cmd, sep=' '):
    return shell_to_str(cmd).split(sep)

def replace(str):
    return str.format(**g['replacements'])

def replace_and_run(str):
    return shell_to_str(replace(str))

def log(str='', _end='\n'):
    print('{}'.format(str), file=g['log'], end=_end)

def tee(str='', _end='\n'):
    print('{}'.format(str), end=_end)
    print('{}'.format(str), file=g['log'], end=_end)

def log_replace_and_run(str):
    log(replace_and_run(str), '')

def tee_replace_and_run(str):
    tee(replace_and_run(str), '')

###############################################################################
###############################################################################
#### BEGIN CONFIG
###############################################################################
###############################################################################

set_dir_compile ( shell_to_str('echo $(pwd)/../../microbench') )    ## working directory where the compilation command should be executed
set_dir_run     ( shell_to_str('echo $(pwd)/bin') )                 ## working directory where your program should be executed
set_dir_data    ( shell_to_str('echo $(pwd)/data') )                ## directory for data files produced by individual runs of your program

##
## add parameters that you want your experiments to be run with.
## your program will be run once for each set of values in the CROSS PRODUCT of all parameters.
## (i.e., we will run your program with every combination of parameters)
##

add_run_param ( '_trial'          , [1, 2, 3] )
add_run_param ( '_distribution'   , ['uniform', 'zipf'] )
add_run_param ( '_insdel'         , ['0.0 0.0', '0.5 0.5', '20.0 10.0'] )
add_run_param ( '_keyrange'       , [2000000, 20000000] )
add_run_param ( '_alg'            , ['brown_ext_ist_lf', 'brown_ext_abtree_lf', 'bronson_pext_bst_occ'] )
add_run_param ( '_nthreads'       , shell_to_list('cd ../../tools ; ./get_thread_counts_numa_nodes.sh') )

add_run_param ( '_thread_pinning' , ['-pin ' + shell_to_str('cd ../../tools ; ./get_pinning_cluster.sh')] )

##
## specify how to compile and run your program.
##
## you can use any of the parameters you defined above to dynamically replace {_tokens_like_this}.
## you can also get the directories saved above by using {__dir_compile} {__dir_run} {__dir_data}.
##
## there are also some replacement tokens already defined for you:
##      {__outfile_run}     the output file for the current run
##      {__step}            the number of runs done so far, printed with up to six leading zeros
##
## your compile command will be executed in the compile directory above.
## your run command will be executed in the run directory above.
##

set_cmd_compile ( 'make -j all bin_dir={__dir_run}' )
set_cmd_run     ( 'LD_PRELOAD=../../../lib/libjemalloc.so timeout 300 numactl --interleave=all time ./ubench_{_alg}.alloc_new.reclaim_debra.pool_none.out -nwork {_nthreads} -nprefill {_nthreads} -dist-{_distribution} -insdel {_insdel} -k {_keyrange} -t 30000 {_pinning} -rq 0 -rqsize 1 -nrq 0' )

set_outfile_run ( 'data{__step}.txt' )                              ## pattern for output filenames. note: filenames cannot contain spaces.

##
## add data fields to be fetched from all output files.
## each of these becomes a column in a table of data.
## a data field XYZ must correspond to a single line in your program output of the form 'XYZ=[...]'
##
## also note: each of these fields also becomes a replacement token, e.g., {PAPI_L3_TCM}.
##

add_data_field ( 'distribution' )
add_data_field ( 'INS_DEL_FRAC' )
add_data_field ( 'MAXKEY' )
add_data_field ( 'DS_TYPENAME' )
add_data_field ( 'TOTAL_THREADS' )
add_data_field ( 'total_throughput' )
add_data_field ( 'PAPI_L2_TCM' )
add_data_field ( 'PAPI_L3_TCM' )
add_data_field ( 'PAPI_TOT_CYC' )
add_data_field ( 'PAPI_TOT_INS' )
add_data_field ( 'maxresident_mb' )
add_data_field ( 'tree_stats_height' )
add_data_field ( 'tree_stats_avgKeyDepth' )
add_data_field ( 'tree_stats_sizeInBytes' )
add_data_field ( 'validate_result' )
add_data_field ( 'MILLIS_TO_RUN' )
add_data_field ( 'RECLAIM' )
add_data_field ( 'POOL' )
add_data_field ( 'cmd' )
add_data_field ( '__outfile_run' )

##
## plots with multiple series
##

add_plots ( filename='plot_throughput-u{INS_DEL_FRAC}-k{MAXKEY}.png'  , title='u={INS_DEL_FRAC} k={MAXKEY}: Throughput vs thread count'     , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='total_throughput', plot_type='bars' )
add_plots ( filename='plot_l2miss-u{INS_DEL_FRAC}-k{MAXKEY}.png'      , title='u={INS_DEL_FRAC} k={MAXKEY}: L2 misses/op vs thread count'   , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_L2_TCM'     , plot_type='bars' )
add_plots ( filename='plot_l3miss-u{INS_DEL_FRAC}-k{MAXKEY}.png'      , title='u={INS_DEL_FRAC} k={MAXKEY}: L3 misses/op vs thread count'   , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_L3_TCM'     , plot_type='bars' )
add_plots ( filename='plot_cycles-u{INS_DEL_FRAC}-k{MAXKEY}.png'      , title='u={INS_DEL_FRAC} k={MAXKEY}: Cycles/op vs thread count'      , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_TOT_CYC'    , plot_type='bars' )
add_plots ( filename='plot_instructions-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Instructions/op vs thread count', groupby_list=['INS_DEL_FRAC', 'MAXKEY'], series='DS_TYPENAME', x_axis='TOTAL_THREADS', y_axis='PAPI_TOT_INS'    , plot_type='bars' )

##
## plots with only one series (no series field defined)
##

add_plots ( filename='plot_maxresident-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Max resident size (MB)', groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='maxresident_mb'        , plot_type='bars' )
add_plots ( filename='plot_avgkeydepth-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Average key depth'     , groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='tree_stats_avgKeyDepth', plot_type='bars' )

##
## example: a plot with data filtered to maximum thread count (only),
##          where the max thread count for this system is computed by a shell script
##

max_threads = shell_to_str('cd ../../tools ; ./get_thread_counts_max.sh')
filter_string = 'TOTAL_THREADS == {}'.format(max_threads)

add_plots ( filename='plot_sizebytes-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Data structure size (bytes)', filter=filter_string, groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='tree_stats_sizeInBytes', plot_type='bars' )

##
## example: a plot with data filtered to single threaded runs
##

add_plots ( filename='plot_sequential-u{INS_DEL_FRAC}-k{MAXKEY}.png', title='u={INS_DEL_FRAC} k={MAXKEY}: Single threaded throughput', filter='TOTAL_THREADS == 1', groupby_list=['INS_DEL_FRAC', 'MAXKEY'], x_axis='DS_TYPENAME', y_axis='total_throughput', plot_type='bars' )

###############################################################################
###############################################################################
#### END CONFIG
###############################################################################
###############################################################################

tee('## Script running with the following parameters:')
pp_stdout.pprint(g)
pp_log.pprint(g)

#########################################################################
#### Backup ONE set of old data and binaries (to avoid mistakes...)
#########################################################################

tee()
tee("## Moving old binaries and data to *.old")
tee()

tee_replace_and_run('rm -rf {__dir_run}.old 2>&1')
tee_replace_and_run('rm -rf {__dir_data}.old 2>&1')
tee_replace_and_run('mv {__dir_run} {__dir_run}.old 2>&1')
tee_replace_and_run('mv {__dir_data} {__dir_data}.old 2>&1')
tee_replace_and_run('mkdir {__dir_run}')
tee_replace_and_run('mkdir {__dir_data}')

# #########################################################################
# #### Check whether we should run in TESTING mode (if there's any argument to this script)
# #########################################################################

# if [ "$#" -ne "0" ]; then
#     echo "WARNING: TESTING mode enabled" | tee $flog
#     num_trials=1
#     millis_to_run=100
#     thread_counts=( "1" $(cd ../../tools ; ./get_thread_counts_max.sh) )
#     prefill=0
# else
#     echo "Note: PRODUCTION mode enabled" | tee $flog
# fi

# #########################################################################
# #### Compile into bin_dir
# #########################################################################

# echo | tee -a $flog
# echo "## Compiling into ${bin_dir}" | tee -a $flog
# echo | tee -a $flog

# ## compile in a subshell to avoid changing directory...
# bash -c "cd ${compile_dir} ; make -j all bin_dir=${bin_dir} 2>&1 >>$flog"
# if [ "$?" -ne "0" ]; then
#     echo "ERROR: compiling... see $flog" | tee -a $flog
#     exit 1
# else
#     echo "Success." | tee -a $flog
# fi

# #########################################################################
# #### Run trials
# #########################################################################

# started=`date`
# echo | tee -a $flog
# echo "## Running trials (starting at $started)" | tee -a $flog
# echo | tee -a $flog

# step=0
# maxstep=$(echo "$num_trials * ${#insert_delete_fractions[@]} * ${#key_range_sizes[@]} * ${#algorithms[@]} * ${#thread_counts[@]}" | bc)

# # echo maxstep=$maxstep
# # echo "multipliers:"
# # echo $num_trials
# # echo ${#insert_delete_fractions[@]}
# # echo ${#key_range_sizes[@]}
# # echo ${#algorithms[@]}
# # echo ${#thread_counts[@]}

# started_sec=$(date +%s)

# printTime() {
#     ## takes seconds as arg
#     s=$1
#     if (($s==0)); then
#         printf "%02ds" "0"
#     fi
#     if (($s>86400)); then
#         printf "%02dd" $(($s/86400))
#         s=$(($s%86400))
#     fi
#     if (($s>3600)); then
#         printf "%02dh" $(($s/3600))
#         s=$(($s%3600))
#     fi
#     if (($s>60)); then
#         printf "%02dm" $(($s/60))
#         s=$(($s%60))
#     fi
#     if (($s>0)); then
#         printf "%02ds" $s
#     fi
# }

# for ((trial=0; trial<num_trials; ++trial)) ; do
#     for ins_del_frac in "${insert_delete_fractions[@]}" ; do
#         ins_frac=$(echo $ins_del_frac | cut -d" " -f1)
#         del_frac=$(echo $ins_del_frac | cut -d" " -f2)

#         for k in "${key_range_sizes[@]}" ; do
#             for alg in "${algorithms[@]}" ; do
#                 for n in "${thread_counts[@]}" ; do

#                     ## actually run current trial
#                     step=$((step+1))
#                     f=$(printf "%s%06d%s.txt" "${data_dir}/data" "$step" "-i${ins_frac}-d${del_frac}-k${k}-${alg}-n${n}")
#                     echo "$step / $maxstep: $f" | tee -a $flog

#                     ## build command string
#                     prefill_str="" ; if [ "$prefill" -eq "1" ]; then prefill_str="-nprefill $n" ; fi
#                     args="-nwork $n $prefill_str -i $ins_frac -d $del_frac -k $k -t $millis_to_run -pin $pinning_policy $other_args"
#                     cmd="LD_PRELOAD=${malloc_lib} timeout $timeout_sec numactl --interleave=all time ${bin_dir}/ubench_${alg}.alloc_new.reclaim_debra.pool_none.out $args"

#                     ## add command details to file
#                     echo "cmd=$cmd" > $f
#                     echo "fname=$f" >> $f
#                     printf "step=%06d\n" "$step" >> $f

#                     ## run the command
#                     eval $cmd >> $f 2>&1
#                     if [ "$?" -ne "0" ]; then
#                         echo "ERROR in run $f" >> $ferr
#                     fi

#                     ## manually parse the maximum resident size from the output of `time` and add it to the data file
#                     maxres=$(echo $(cat $f | grep "maxres" | cut -d" " -f6 | cut -d"m" -f1) / 1000 | bc)
#                     echo "maxresident_mb=$maxres" >> $f

#                     ## verify throughput measurement is present and positive
#                     throughput=$(cat $f | grep "^total_throughput=" | cut -d"=" -f2)
#                     if [ "$throughput" == "" ] || [ "$throughput" -le "0" ]; then
#                         echo "WARNING: positive throughput NOT found in $f" >> $ferr
#                     fi

#                     ## update progress info
#                     curr_sec=$(date +%s)
#                     elapsed_sec=$(echo "scale=0; $curr_sec - $started_sec" | bc)
#                     frac_done=$(echo "scale=3; $step / $maxstep" | bc)
#                     elapsed_sec_per_done=$(echo "scale=3; $elapsed_sec / $step" | bc)
#                     remaining_sec=$(echo "scale=0; ($elapsed_sec_per_done * ($maxstep - $step)) / 1" | bc)
#                     total_est_sec=$(echo "scale=0; $elapsed_sec + $remaining_sec" | bc)

#                     echo -n "    (" | tee -a $flog
#                     printTime $elapsed_sec | tee -a $flog
#                     echo -n " elapsed" | tee -a $flog
#                     echo -n ", " | tee -a $flog
#                     printTime $remaining_sec | tee -a $flog
#                     echo -n " est. remaining" | tee -a $flog
#                     echo -n ", " | tee -a $flog
#                     echo -n "out of est. total " | tee -a $flog
#                     printTime $total_est_sec | tee -a $flog
#                     echo ")" | tee -a $flog

#                     ## add host info to f
#                     echo "hostname=${host}" >> $f

#                 done
#             done
#         done
#     done
# done

# echo | tee -a $flog
# echo "started: $started" | tee -a $flog
# echo "finished: $(date)" | tee -a $flog

# echo | tee -a $flog
# echo "## Zipping results into ./${host}_data.zip" | tee -a $flog
# echo | tee -a $flog
# zip -r ./${host}_data.zip *.sh ${compile_dir}/Makefile ${data_dir} >> $flog 2>&1

# echo | tee -a $flog
# echo "Log can be viewed at $flog"

# if [ -e "$ferr" ] && [ "0$(cat $ferr | wc -l)" -ne "0" ]; then
#     echo
#     echo "#########################################################################"
#     echo "#### ERROR: see $ferr"
#     echo "#########################################################################"
#     echo
# fi
