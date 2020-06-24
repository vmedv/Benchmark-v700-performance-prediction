#!/bin.bash

#################################
## fill the following arrays with field names to get the desired results:
##
## fdecorative will be included in the CSV, but not plotted.
## fpages will determine which experimental configurations are plotted.
## fseries will determine the series of the plots.
## faxis will determine the x-axis of the plots.
## fvalues will determine the y-axes of various plots.
##
## a plot will be produced for each unique *set* of values taken on by
## the (set of) fields in fpages, as well as each field in fvalues.
##
## in other words, imagine you filter your data to the fields in fpages,
## then sort and unique that data to obtain rows {r1, r2, ...}, then take
## the cross product of {r1, r2, ...} and {fvalues[0], fvalues[1], ...}.
## there will be a plot for each element of that cross product.
## each such plot will have series fseries[0], x-axis faxis[0], and
##   the y-axis will be one of {fvalues[0], fvalues[1], ...}.
##
## you should also provide a short name for each field in fpages
##   (for inclusion in filenames, instead of the long form).
## if you want to merge some fpages fields in filenames, just skip
##   the leading "-" in the fpages_short name.
#################################

## note: could make pagination explicit here with page_row and page_col

fdecorative+=( step )

fpages+=( hostname )
fpages+=( INS )
fpages+=( DEL )
fpages+=( MAXKEY )

fseries+=( DS_TYPENAME )

faxis+=( TOTAL_THREADS )

fvalues+=( total_throughput )
fvalues+=( PAPI_L2_TCM )
fvalues+=( PAPI_L3_TCM )
fvalues+=( PAPI_TOT_CYC )
fvalues+=( PAPI_TOT_INS )
fvalues+=( maxresident_mb )

fpages_short=( "-" "-i" "d" "-k" )

#################################
## no need to edit below here
#################################

fields+=( ${fdecorative[@]} )
fields+=( ${fpages[@]} )
fields+=( ${fseries[@]} )
fields+=( ${faxis[@]} )
fields+=( ${fvalues[@]} )

num_fdecorative=${#fdecorative[@]}
num_fpages=${#fpages[@]}
num_fvalues=${#fvalues[@]}
num_fields=${#fields[@]}
ix_fpages_start=$num_fdecorative
ix_fpages_end=$(($ix_fpages_start + $num_fpages - 1))
ix_fseries=$(($ix_fpages_end + 1))
ix_faxis=$(($ix_fseries + 1))
ix_fvalues_start=$(($ix_faxis + 1))
ix_fvalues_end=$(($ix_fvalues_start + $num_fvalues - 1))

echo "ix_fpages_start=$ix_fpages_start ix_fpages_end=$ix_fpages_end ix_fseries=$ix_fseries ix_faxis=$ix_faxis ix_fvalues_start=$ix_fvalues_start ix_fvalues_end=$ix_fvalues_end"
