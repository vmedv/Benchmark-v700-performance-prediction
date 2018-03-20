#!/bin/bash
#
# Run script for microbenchmarks.
#
# Author: Trevor Brown

source ../plaf.inc

trials=5

cols="%6s %12s %24s %8s %6s %6s %8s %6s %6s %8s %12s %12s %12s %12s"
headers="step machine ds k u rq rqsize nrq nwork trial throughput rqs updates finds"
machine=`hostname`

echo "Generating 'experiment_list.txt' according to settings in 'plaf.inc' and 'experiment_list_generate.sh'..."
./experiment_list_generate.sh

outdir=data
fsummary=$outdir/summary.txt

rm -r -f $outdir.old
mv -f $outdir $outdir.old
mkdir $outdir

rm -f warnings.txt

if [ "$1" == "test" ] ; then
    testingmode=1
    millis=1
    prefill_and_time="-t ${millis}"
else
    testingmode=0
    millis=3000
    prefill_and_time="-p -t ${millis}"
fi

skip_steps_before=0
skip_steps_after=1000000
if [ "$#" -eq "2" ] ; then
    skip_steps_before=$1
    skip_steps_after=$2
fi

cnt2=`cat experiment_list.txt | wc -l`
cnt2=`expr $cnt2 \* $trials`
echo "Performing $cnt2 trials..."

secs=`expr $millis / 1000`
estimated_secs=`expr $cnt2 \* $secs`
estimated_hours=`expr $estimated_secs / 3600`
estimated_mins=`expr $estimated_secs / 60`
estimated_mins=`expr $estimated_mins % 60`
echo "(Very rough) estimate of running time (+/- 20%): ${estimated_hours}h${estimated_mins}m" > $fsummary

cnt1=100000
cnt2=`expr $cnt2 + 100000`

source ../sys_config_checks.inc

printf "${cols}" ${headers} >> $fsummary
cat $fsummary ; echo
printf " %20s %s\n" fname cmd >> $fsummary

while read u rq rqsize k nrq nwork ds bin ; do
    for ((trial=0;trial<$trials;++trial)) ; do
        cnt1=`expr $cnt1 + 1`
        if ((cnt1 < skip_steps_before)); then continue; fi
        if ((cnt1 > skip_steps_after)); then continue; fi

        fname="$outdir/step$cnt1.out"
        cmd="$bin -i $u -d $u -k $k -rq $rq -rqsize $rqsize ${prefill_and_time} -nrq $nrq -nwork $nwork ${pinning_policy}"
        echo "env LD_PRELOAD=${jemalloc_path} $cmd" > $fname
        echo "step=$cnt1" >> $fname
        echo "machine=$machine" >> $fname
        echo "trial=$trial" >> $fname
        echo "binary=$bin" >> $fname
        echo "data_structure=$ds" >> $fname
        env LD_PRELOAD=${jemalloc_path} $cmd >> $fname
        printf "${cols}" \
                $cnt1 $machine $ds $k $u $rq $rqsize $nrq $nwork $trial \
                "`cat $fname | grep 'total throughput' | cut -d':' -f2`" \
                "`cat $fname | grep 'total rq' | cut -d':' -f2`" \
                "`cat $fname | grep 'total updates' | cut -d':' -f2`" \
                "`cat $fname | grep 'total find' | cut -d':' -f2`" >> $fsummary
        tail -1 $fsummary
        echo
        printf "%20s %s\n" "$fname" "`head -1 $fname`" >> $fsummary

        throughput=`cat $fname | grep "total throughput" | cut -d":" -f2 | tr -d " "`
        if [ "$throughput" == "" ] || [ "$throughput" -le "0" ] ; then echo "WARNING: thoughput $throughput in file $fname" >> warnings.txt ; cat warnings.txt | tail -1 ; fi
    done
done < experiment_list.txt

if [ "`cat warnings.txt | wc -l`" -ne 0 ]; then
    echo "NOTE: THERE WERE WARNINGS. PRINTING THEM..."
    cat warnings.txt
fi

echo "Creating database of results from raw output files..."
mv results.db results.db.old
python create_db.py
echo "Finished."
echo "(The database can be accessed as in graphs.py or by using convert_db_to_csv.py to obtain a CSV file.)"
