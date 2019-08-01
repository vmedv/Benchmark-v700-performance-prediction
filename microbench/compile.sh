#!/bin/sh

hwthreads=`lscpu | grep -e "^CPU(s):" | cut -d":" -f2 | tr -d " "`
#echo "hwthreads=$hwthreads"
use=`expr $hwthreads - 1`
#echo "make -j $use all"
make -j $use all
