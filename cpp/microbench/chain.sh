#!/usr/bin/env bash

set -euo pipefail

DSs=("aksenov_splaylist_64" "bronson_pext_bst_occ" "brown_ext_abtree_lf" "drachsler_pext_bst_lock" "ellen_ext_bst_lf" "guerraoui_ext_bst_ticket" "howley_int_bst_lf")

for ds in "${DSs[@]}"
do
  for it in {1..5}
  do
    ./run.sh $ds $it
  done
done
