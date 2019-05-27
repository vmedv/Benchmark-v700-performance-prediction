#!/bin/bash

if [ "$#" -le "0" ]; then
	echo "USAGE: ./parse.sh [input files...]"
	exit 1
fi

fields="DS_TYPENAME size_node RECLAIM ALLOC POOL MILLIS_TO_RUN INS DEL RQ RQSIZE MAXKEY"
fields="$fields PREFILL_THREADS TOTAL_THREADS WORK_THREADS RQ_THREADS INSERT_FUNC"
fields="$fields threads_final_keysum threads_final_size final_keysum final_size validate_result"
fields="$fields tree_stats_height tree_stats_numInternals tree_stats_numLeaves tree_stats_numNodes"
fields="$fields tree_stats_numKeys tree_stats_avgDegreeInternal tree_stats_avgDegreeLeaves"
fields="$fields tree_stats_avgDegree tree_stats_avgKeyDepth"
fields="$fields total_find total_rq total_updates total_queries total_ops"
fields="$fields find_throughput rq_throughput update_throughput query_throughput total_throughput"
fields="$fields PAPI_L1_DCM PAPI_L2_TCM PAPI_L3_TCM PAPI_TOT_CYC PAPI_TOT_INS"
fields="$fields maxresident_mb prefill_elapsed_ms"

outfile=data.csv
echo "filename $fields" | tr " " "," > $outfile ; tail -1 $outfile
for fname in $@ ; do
    if [ -e "$fname" ]; then
	echo -n $fname >> $outfile
	for f in $fields ; do
		val=`cat $fname | grep "^${f}=" | cut -d"=" -f2`
		echo -n ,$val >> $outfile
	done

	echo "" >> $outfile ; tail -1 $outfile
    fi
done
