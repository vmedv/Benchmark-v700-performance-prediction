#!/bin/sh

hwthreads=`lscpu | grep "CPU(s):" | cut -d":" -f2 | tr -d " "`
#echo "hwthreads=$hwthreads"
use=`expr $hwthreads - 2`
#echo "make -j $use all"
make -j $use all
