#!/bin/bash

add_run_param() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

add_data_field() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

add_series_plots() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

add_plots() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

set_dir_compile() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo
    dir_compile=$1
}

set_dir_run() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo
    dir_run=$1
}

set_dir_data() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo
    dir_data=$1
}

## does replacement
set_cmd_compile() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

## does replacement
set_cmd_run() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

## does replacement
set_outfile_run() {
    echo -n call ${FUNCNAME[0]} ; for ((i=1;i<=$#;++i)) ; do echo -n " \"${!i}\"" ; done ; echo

}

get_dir_compile() {
    echo $dir_compile
}
export -f get_dir_compile

get_dir_run() {
    echo $dir_run
}
export -f get_dir_run

get_dir_data() {
    echo $dir_data
}
export -f get_dir_data


###############################################################################
###############################################################################
#### BEGIN CONFIG
###############################################################################
###############################################################################

##
## add parameters that you want your experiments to be run with.
## your program will be run once for each set of values in the CROSS PRODUCT of all parameters.
## (i.e., we will run your program with every combination of parameters)
##

add_run_param       _trial                      1 2 3
add_run_param       _distribution               uniform zipf
add_run_param       _insdel                     "0.0 0.0" "0.5 0.5" "20.0 10.0"
add_run_param       _keyrange                   2000000 20000000
add_run_param       _alg                        brown_ext_ist_lf brown_ext_abtree_lf bronson_pext_bst_occ
add_run_param       _nthreads                   $(cd ../../tools ; ./get_thread_counts_numa_nodes.sh)

set_dir_compile     $(pwd)/../../microbench     ## working directory where the compilation command should be executed
set_dir_run         $(pwd)/bin                  ## working directory where your program should be executed
set_dir_data        $(pwd)/data                 ## directory for data files produced by individual runs of your program

##
## specify how to compile and run your program.
##
## you can use any of the parameters you defined above to dynamically replace {{_tokens_like_this}}.
## you can also get the directories saved above by using $(get_dir_compile) $(get_dir_run) $(get_dir_data).
##
## there are also some replacement tokens already defined for you:
##      __outfile_run   the output file for the current run
##      __step          the number of runs done so far, printed with up to six leading zeros
##
## your compile command will be executed in the compile directory above.
## your run command will be executed in the run directory above.
##

set_cmd_compile     "make -j all bin_dir=$(get_dir_run)"
set_cmd_run         "LD_PRELOAD=../../../lib/libjemalloc.so timeout 300 numactl --interleave=all time $(get_dir_run)/ubench_{{_alg}}.alloc_new.reclaim_debra.pool_none.out -nwork {{_nthreads}} -nprefill {{_nthreads}} -dist-{{_distribution}} -insdel {{_insdel}} -k {{_keyrange}} -t 30000 -pin $(cd ../../tools ; ./get_thread_counts_numa_nodes.sh) -rq 0 -rqsize 1 -nrq 0"
set_outfile_run     "data{{__step}}.txt"        ## pattern for output filenames. note: filenames cannot contain spaces.

##
## add data fields to be fetched from all output files.
## each of these becomes a column in a table of data.
## a data field XYZ must correspond to a single line in your program output of the form "XYZ=[...]"
##
## also note: each of these fields also becomes a replacement token, e.g., {{PAPI_L3_TCM}}.
##

add_data_field      distribution
add_data_field      INS_DEL_FRAC
add_data_field      MAXKEY
add_data_field      DS_TYPENAME
add_data_field      TOTAL_THREADS
add_data_field      total_throughput
add_data_field      PAPI_L2_TCM
add_data_field      PAPI_L3_TCM
add_data_field      PAPI_TOT_CYC
add_data_field      PAPI_TOT_INS
add_data_field      maxresident_mb
add_data_field      tree_stats_height
add_data_field      tree_stats_avgKeyDepth
add_data_field      tree_stats_sizeInBytes
add_data_field      validate_result
add_data_field      MILLIS_TO_RUN
add_data_field      RECLAIM
add_data_field      POOL
add_data_field      cmd
add_data_field      __outfile_run

## future improvement: add_data_field_fn    additionally takes a function name (which takes two args: infile and fieldname); this function reads the file and extracts the desired value from it, echoing to return it.

##
## plots with multiple series
##

##                  filename prefix                                     plot title                                                          filter      groupby fields          series field    x-axis field    y-axis field            type    plot_cmd args
add_series_plots    "plot_throughput-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"     "u={{INS_DEL_FRAC}} k={{MAXKEY}}: Throughput vs thread count"       ""          "INS_DEL_FRAC MAXKEY"   DS_TYPENAME     TOTAL_THREADS   total_throughput        bars    ""
add_series_plots    "plot_l2miss-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"         "u={{INS_DEL_FRAC}} k={{MAXKEY}}: L2 misses/op vs thread count"     ""          "INS_DEL_FRAC MAXKEY"   DS_TYPENAME     TOTAL_THREADS   PAPI_L2_TCM             bars    ""
add_series_plots    "plot_l3miss-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"         "u={{INS_DEL_FRAC}} k={{MAXKEY}}: L3 misses/op vs thread count"     ""          "INS_DEL_FRAC MAXKEY"   DS_TYPENAME     TOTAL_THREADS   PAPI_L3_TCM             bars    ""
add_series_plots    "plot_cycles-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"         "u={{INS_DEL_FRAC}} k={{MAXKEY}}: Cycles/op vs thread count"        ""          "INS_DEL_FRAC MAXKEY"   DS_TYPENAME     TOTAL_THREADS   PAPI_TOT_CYC            bars    ""
add_series_plots    "plot_instructions-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"   "u={{INS_DEL_FRAC}} k={{MAXKEY}}: Instructions/op vs thread count"  ""          "INS_DEL_FRAC MAXKEY"   DS_TYPENAME     TOTAL_THREADS   PAPI_TOT_INS            bars    ""

##
## plots with only one series
##

##                  filename prefix                                     plot title                                                          filter      groupby fields                          x-axis field    y-axis field            type    plot_cmd args
add_plots           "plot_maxresident-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"    "u={{INS_DEL_FRAC}} k={{MAXKEY}}: Max resident size (MB)"           ""          "INS_DEL_FRAC MAXKEY"                   DS_TYPENAME     maxresident_mb          bars    ""
add_plots           "plot_avgkeydepth-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"    "u={{INS_DEL_FRAC}} k={{MAXKEY}}: Average key depth"                ""          "INS_DEL_FRAC MAXKEY"                   DS_TYPENAME     tree_stats_avgKeyDepth  bars    ""

##
## example: plots filtered to one thread count
##

threadcount=$(cd ../../tools ; ./get_thread_counts_max.sh)

##                  filename prefix                                     plot title                                                          filter                              groupby fields          x-axis field    y-axis field            type    plot_cmd args
add_plots           "plot_sizebytes-u{{INS_DEL_FRAC}}-k{{MAXKEY}}"      "u={{INS_DEL_FRAC}} k={{MAXKEY}}: Data structure size (bytes)"      "TOTAL_THREADS == $threadcount"   "INS_DEL_FRAC MAXKEY"   DS_TYPENAME     tree_stats_sizeInBytes  bars    ""

###############################################################################
###############################################################################
#### END CONFIG
###############################################################################
###############################################################################
