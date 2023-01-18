#!/bin/bash

synFolder='/mnt/tank/scratch/rgaliev/synchrobench/java/'
#synFolder='/mnt/c/Users/Huawei/OneDrive/Документы/ИТМО/LAB/synchrobench/java/'
resultFolder='synchrobench_tests_results/'

#args='-fo -t 16 -i 8192 -r 16384 -d 60000 '
#version='_16_fs_2'

# -t
numThreads=("1" "2" "4" "8" "16" "32" "64")
# -u
numWrites=("0" "2" "10" "20")
# -r
rangeAndSizes=("100000 -i 50000") # "100000 -i 50000" "1000000 -i 500000")
ranges=("100000") # "100000" "1000000")
# -d
timeD="10000"
# -b
algoFolder="trees.lockbased."
algos=("LockBasedFriendlyTreeMap" "LockBasedStanfordTreeMap")

for algo in ${algos[@]}; do
  for numThread in ${numThreads[@]}; do
    for numWrite in ${numWrites[@]}; do
      for ri in ${!rangeAndSizes[@]}; do
        main=' -cp '$synFolder'bin contention.benchmark.Test -b '$algoFolder$algo
        args=' -t '$numThread' -u '$numWrite' -r '${rangeAndSizes[ri]}' -d '$timeD' '
        file=$synFolder$resultFolder$algoFolder$algo'/'$algo'-t'$numThread'-u'$numWrite'-r'${ranges[ri]}'-d'$timeD'.txt'

        STest=$main$args

        java $STest > $file
        echo
      done
    done
  done
done
