args='-i 5 -d 5 -k 100000 -rq 0 -nprefill 8 -t 10000 -nrq 0 -nwork 8 '
argsrw='-dist-skewed-sets -rx 0.9 -ry 0.1 -wx 0.9 -wy 0.2 -inter 0.05'

LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra $args$argsrw
