#!/usr/bin/env bash

set -euo pipefail

TRIALS=5

structs=(
  "aksenov_splaylist_64" 
  "bronson_pext_bst_occ" 
  "brown_ext_abtree_lf" 
  "brown_ext_bslack_rq_lf" 
  "ellen_ext_bst_lf" 
  "guerraoui_ext_bst_ticket" 
  "drachsler_pext_bst_lock" 
  "howley_int_bst_lf"
)

dists=(
  "uniform"
)

modes=(
  "r"
)

threads=(
  "1"
  "2c"
  "2w"
  "2n"
)

for DS in "${structs[@]}"
do
  for DIST in "${dists[@]}"
  do
    for MODE in "${modes[@]}"
    do
      for THREADS in "${threads[@]}"
      do 
        for it in $(seq 1 $TRIALS);
        do
          DIST="$DIST" MODE="$MODE" THREADS="$THREADS" DS="$DS" bash -c 'LIBPFM_FORCE_PMU=amd64 LD_PRELOAD=../../../lib/libmimalloc.so \
          ../../bin/${DS}.debra -create-default-prefill -json-file ${DIST}-${MODE}-${THREADS}.json -cache-file out.csv'
        done
      done
    done
  done
done
