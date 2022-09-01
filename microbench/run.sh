args='-i 5 -d 5 -k 100000 -rq 0 -nprefill 8 -t 10000 -nrq 0 -nwork 8 '
argsSS='-dist-skewed-sets -rx 0.9 -ry 0.1 -wx 0.9 -wy 0.2 -inter 0.05'
argsTS='-dist-temporary-skewed -set-count 2 -rt 1000 -ht 1000 -xi 0 0.2 -xi 1 0.3 -yi 0 0.9 -yi 1 0.8 -ti 1 900 -rti 1 1200'


LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra $args$argsTS

#LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra -i 0 -d 0 -k 100000 -rq 0 -nprefill 8 -t 10000 -nrq 0 -nwork 8 -dist-temporary-skewed -set-count 2 -rt 1000 -ht 1000 -xi 0 0.2 -xi 1 0.3 -yi 0 0.9 -yi 1 0.8 -ti 1 900 -rti 1200
