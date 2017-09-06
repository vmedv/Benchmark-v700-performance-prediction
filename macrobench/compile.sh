#!/bin/bash
# 
# File:   compile.sh
# Author: trbot
#
# Created on May 28, 2017, 9:56:43 PM
#

workloads="TPCC"
modes="withupdates"

algs=""

#algs+=" BST ABTREE"

algs+=" BST_RQ_LOCKFREE BST_RQ_RWLOCK BST_RQ_HTM_RWLOCK BST_RQ_UNSAFE"
algs+=" CITRUS_RQ_LOCKFREE CITRUS_RQ_RWLOCK CITRUS_RQ_HTM_RWLOCK CITRUS_RQ_UNSAFE"
algs+=" ABTREE_RQ_LOCKFREE ABTREE_RQ_RWLOCK ABTREE_RQ_HTM_RWLOCK ABTREE_RQ_UNSAFE"
algs+=" SKIPLISTLOCK_RQ_LOCKFREE SKIPLISTLOCK_RQ_RWLOCK SKIPLISTLOCK_RQ_HTM_RWLOCK SKIPLISTLOCK_RQ_UNSAFE"
#algs+=" SKIPLISTLOCK_RQ_SNAPCOLLECTOR"
algs+=" RLUCITRUS"

#algs+=" BST ABTREE HASH"
#algs+=" HOWLEY HOWLEY_PAD HOWLEY_PAD_LARGE_DES"
#algs+=" ELLEN ELLEN_PAD"
#algs+=" BRONSON_SPIN"
#algs+=" CCAVL_SPIN"
#algs+=" DANA_SPIN_FIELDS DANA_SPIN_PAD_FIELDS DANA_SPIN_FIELDS_3_LINES"
#algs+=" INTLF INTLF_PAD"
#algs+=" TICKET TICKET_PAD"
#algs+=" WFRBT WFRBT_ASCY"
#algs+=" CITRUS_SPIN CITRUS_SPIN_PAD"
#algs+=" HOWLEY_BASELINE ELLEN_BASELINE BRONSON_BASELINE CCAVL_BASELINE DANA_BASELINE INTLF_BASELINE TICKET_BASELINE WFRBT_ASCY_BASELINE CITRUS_BASELINE"
#algs+=" BRONSON_SPIN_NO_REREAD BRONSON_SPIN_NO_OVL CCAVL_SPIN_NO_REREAD CCAVL_SPIN_NO_OVL"

make_workload_dict() {
    # skip the readonly variant of the YCSB workload
    # (we only care about having a special read only TPCC workload)
    if [ "$1" == "YCSB" ] && [ "$3" == "readonly" ]; then exit 0 ; fi

    # compile the given workload, algorithm and mode
    ro=""
    if [ "$3" == "readonly" ]; then ro="readonly=-DREAD_ONLY" ; fi
    make -j clean workload=$1 dict=$2 $ro
    make -j workload=$1 dict=$2 $ro &> compiling.$1.$2.$3.out
    if [ $? -ne 0 ]; then
        echo "Compilation FAILED for $1 $2 $3"
    else
        echo "Compiled $1 $2 $3"
        rm -f compiling.$1.$2.$3.out
    fi
}
export -f make_workload_dict

rm -f compiling.*.out

# check for gnu parallel
command -v parallel >/dev/null 2>&1
if [ "$?" -eq "0" ]; then
	parallel make_workload_dict ::: $workloads ::: $algs ::: $modes
else
	for workload in $workloads; do
	for alg in $algs; do
	for mode in $modes; do
		make_workload_dict $workload $alg $mode
	done
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
