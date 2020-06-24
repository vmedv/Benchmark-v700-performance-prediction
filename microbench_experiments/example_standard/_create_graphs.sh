#!/bin/bash

fin=data.csv

######################
## get unique experiments / pages
######################

source _fields.inc

## validate that data contains the expected fields
expheader="${fields[@]}"
header=$(head -1 $fin)
if [ "$header" != "$expheader" ]; then
    echo "UNEXPECTED HEADER $header"
    exit 1
fi

## get a list of "experiment" columns in a format that awk can print (i.e., "$2, $3, ...")
cols=""
for ((i=$ix_fpages_start; i<=$ix_fpages_end; ++i)) ; do
    cols="$cols \" \" \$$((1+i))"
done

######################
## create graphs
######################

outdir=output
rm -rf $outdir
mkdir $outdir

## iterate over unique experiments
cmds=()
cnt=0
while read -r line ; do
    # echo "line='$line'"

    i=0
    filestr=""
    for w in $line ; do
        filestr="${filestr}${fpages_short[$i]}${w} "
        i=$((1+i))
    done
    imgsuffix=$(echo $filestr | tr -d " ").png
    echo "create commands for graphs suffixed $imgsuffix"

    searchstr="$line"

    ## create graph for each value field
    otherargs="--legend-include --legend-columns 3 --width 12 --height 8"
    for ((i=$ix_fvalues_start; i<=$ix_fvalues_end; ++i)) ; do
        graph_type=${fields[$i]}
        cmds[$cnt]="grep '$searchstr ' $fin | awk '{ print \$$((1+${ix_fseries})), \$$((1+${ix_faxis})), \$$((1+${i})) }' | ../../tools/plotbars.py -o ${outdir}/${graph_type}${imgsuffix} -t '${graph_type} vs ${faxis[0]}' $otherargs"
        # echo "cmds[$cnt]=\"${cmds[$cnt]}\""
        cnt=$((cnt+1))
    done

done < <(awk "{print $cols}" $fin | tail +2 | sort | uniq)

## preview parallel commands

for cmd in "${cmds[@]}" ; do
    echo "cmd=\"$cmd\""
done

## run parallel commands

echo
echo "running..."
parallel ::: "${cmds[@]}"

## verify number of images created

numpng=`ls ${outdir}/*.png | wc -l`
if [ "$numpng" != "$cnt" ]; then
    echo
    echo "######################################################"
    echo "ERROR: we were supposed to create $cnt new images, but there are $numpng in ${outdir}"
    echo "######################################################"
    echo
    exit 1
fi
