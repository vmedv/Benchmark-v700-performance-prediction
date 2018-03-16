#!/bin/sh
# 
# File:   explore_scalability.sh
# Author: tbrown
#
# Created on Mar 16, 2018, 12:13:04 PM
#

cols="%6s %6s %14s %6s\n"
printf "$cols" upd thr throughput scaling
for u in 0 1 5 10 15 ; do
    for n in 1 18 36 72 108 144 ; do
        throughput=`LD_PRELOAD=../lib/libjemalloc-5.0.1-25.so ./brown_ext_abtree_lf.ubench.out -i $u -d $u -rq 0 -rqsize 100 -k 1000000 -nrq 0 -nwork $n -t 1000 -p -bind 0-17,72-89,18-35,90-107,36-53,108-125,54-71,126-143 | grep "total throughput" | cut -d":" -f2 | tr -d " "`
        if ((n == 1)); then base=$throughput ; fi
        scaling=`expr $throughput / $base `
        printf "$cols" $u $n $throughput $scaling
    done
done

#cols="%6s %6s %14s %6s\n" ; printf "$cols" upd thr throughput scaling ; for u in 0 1 5 10 15 ; do for n in 1 18 36 72 108 144 ; do throughput=`LD_PRELOAD=../lib/libjemalloc-5.0.1-25.so ./brown_ext_abtree_lf.ubench.out -i $u -d $u -rq 0 -rqsize 100 -k 1000000 -nrq 0 -nwork $n -t 1000 -p -bind 0-17,72-89,18-35,90-107,36-53,108-125,54-71,126-143 | grep "total throughput" | cut -d":" -f2 | tr -d " "` ; if ((n == 1)); then base=$throughput ; fi ; scaling=`expr $throughput / $base ` ; printf "$cols" $u $n $throughput $scaling ; done ; done