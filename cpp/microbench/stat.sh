#!/usr/bin/env bash

set -euo pipefail

DSs=("aksenov_splaylist_64" "bronson_pext_bst_occ" "brown_ext_abtree_lf" "brown_ext_bslack_rq_lf" "sigouin_int_bst_kcas")

for ds in "${DSs[@]}"
do
  for it in {1..1}
  do
    ./run-stat.sh $ds $it
  done
done
