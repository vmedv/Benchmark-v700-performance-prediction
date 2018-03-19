#!/bin/bash
# 
# File:   compile.sh
# Author: trbot
#
# Created on May 28, 2017, 9:56:43 PM
#

workloads="TPCC YCSB"

algs=( \
    "hash_chaining" \
    "bronson_pext_bst_occ" \
    "brown_ext_abtree_lf" \
    "brown_ext_bslack_lf" \
    "brown_ext_bst_lf" \
    "natarajan_ext_bst_lf" \
)

algs_opts=( \
    "-DIDX_HASH=1" \
    "" \
    "-DUSE_RANGE_QUERIES -DRQ_UNSAFE" \
    "-DUSE_RANGE_QUERIES -DRQ_UNSAFE" \
    "-DUSE_RANGE_QUERIES -DRQ_UNSAFE" \
    "" \
)

make_workload_dict() {
    # compile the given workload and algorithm
    make -j clean workload=$1 data_structure_name=$2 data_structure_opts="$3"
    make -j workload=$1 data_structure_name=$2 data_structure_opts="$3" &> compiling.$1.$2.out
    if [ $? -ne 0 ]; then
        echo "Compilation FAILED for $1 $2 $3"
    else
        echo "Compiled $1 $2 $3"
        rm -f compiling.$1.$2.out
    fi
}
export -f make_workload_dict

rm -f compiling.*.out

# check for gnu parallel
command -v parallel >/dev/null 2>&1
if [ "$?" -eq "0" ]; then
	parallel make_workload_dict ::: $workloads ::: "${algs[@]}" :::+ "${algs_opts[@]}"
else
	for workload in $workloads; do
	for ix in ${!algs[@]}; do
		make_workload_dict $workload ${algs[ix]} ${algs_opts[ix]}
	done
	done
fi

errorfiles=`ls compiling.*.out 2> /dev/null`
numerrorfiles=`ls compiling.*.out 2> /dev/null | wc -l`
if [ "$numerrorfiles" -ne "0" ]; then
    cat compiling.*
    echo "ERROR: some compilation command(s) failed. See the following file(s)."
    for x in $errorfiles ; do echo $x ; done
else
    echo "Compilation successful."
fi
