#!/bin/sh

t=10000 step=10000 a=jemalloc r=debra p=none n=192 f=temp.txt ; LD_PRELOAD=../../../lib/lib$a.so ../../bin/ubench_brown_ext_ist_lf.alloc_new.reclaim_$r.pool_$p.out -nwork $n -nprefill $n -i 50 -d 50 -rq 0 -rqsize 1 -k 20000000 -nrq 0 -t $t -pin 0-23,96-119,24-47,120-143,48-71,144-167,72-95,168-191 > $f ; cat $f | grep "total through" ; cat $f | grep "timeline_" | cut -d" " -f1 | sort | uniq -c

type="timeline_helpRebuild" ; cat temp.txt | grep "$type" | cut -d" " -f2- | tr -d "=_a-zA-Z" > timeline_input.txt ; python $type.py timeline_input.txt

type="timeline_freeSubtree" ; cat temp.txt | grep "$type" | cut -d" " -f2- | tr -d "=_a-zA-Z" > timeline_input.txt ; python $type.py timeline_input.txt

type="timeline_rotateEpochBags" ; cat temp.txt | grep "$type" | cut -d" " -f2- | tr -d "=_a-zA-Z" > timeline_input.txt ; python $type.py timeline_input.txt
