#!/bin/bash

synFolder='/mnt/tank/scratch/rgaliev/synchrobench/java/'
#synFolder='/mnt/c/Users/Huawei/OneDrive/Документы/ИТМО/LAB/synchrobench/java/'
resultFolder='synchrobench_tests_results/'

#args='-fo -t 16 -i 8192 -r 16384 -d 60000 '
#version='_16_fs_2'

# -t
numThreads=("1" "2" "4" "8" "16" "24" "36" "48")
# -u
numWrites=("0" "2" "10" "20")
# -r
rangeAndSizes=("100000 -i 50000") # "100000 -i 50000" "1000000 -i 500000")
ranges=("100000")                 # "100000" "1000000")
# -d
timeD="10000"

# workload
workload="-temporary-skewed"


# -set-count
setCounts=("2" "4")
# -ht -rt
ht_rt_s=("-ht 10000 -rt 3000" "-ht 100000 -rt 3000" "-ht 1000000 -rt 300000")
# -si
sis=("-si 0 0.3 -si 1 0.3" "-si 0 0.2 -si 1 0.2 -si 2 0.2 -si 3 0.2")
# -pi
pis=("-pi 0 0.9 -pi 1 0.9" "-pi 0 0.9 -pi 1 0.9 -pi 2 0.9 -pi 3 0.9")
# -b
algoFolder="trees.lockbased."
algos=("LockBasedFriendlyTreeMap" "LockBasedStanfordTreeMap")

for algo in ${algos[@]}; do
  for numThread in ${numThreads[@]}; do
    for numWrite in ${numWrites[@]}; do
      for ri in ${!rangeAndSizes[@]}; do
        for sti in ${!setCounts[@]}; do
          main=' -cp '$synFolder'bin contention.benchmark.Test -b '$algoFolder$algo
          args=' '$workload' -t '$numThread' -u '$numWrite' -r '${rangeAndSizes[ri]}' -d '$timeD' '
          args=$args' -set-count'${setCounts[sti]}' '${ht_rt_s[sti]}' '${sis[sti]}' '${pis[sti]}' '

          file=$synFolder$resultFolder$algoFolder$algo'/'$algo'_'$workload'-t'$numThread'-u'$numWrite'-r'${ranges[ri]}'-d'$timeD'.txt'

          STest=$main$args

          java $STest >$file

          echo $file' just finished'
        done
      done
    done
  done
done
