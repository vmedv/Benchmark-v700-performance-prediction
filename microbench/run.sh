args=' -i 5 -d 5 -rq 0 -k 10000000 -prefillsize 100000 -nprefill 8 -t 10000 -nrq 0 -nwork 8 -prefill-insert'
argsSS=' -dist-skewed-sets -rp 0.9 -rs 0.1 -wp 0.9 -ws 0.2 -inter 0.05'
argsTS=' -dist-temporary-skewed -set-count 2 -rt 1000 -ht 1000 -xi 0 0.2 -xi 1 0.3 -yi 0 0.9 -yi 1 0.8 -hti 1 900 -rti 1 1200'
argsCW=' -creakers-and-wave -gs 0.03 -gp 0.1 -ws 0.05 -g-age 100000'

LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra $argsCW$args
# chmod -R a+rwx ./run.sh
#LD_PRELOAD=../lib/libjemalloc.so ./bin/aksenov_splaylist_64.debra -i 0 -d 0 -k 100000 -rq 0 -nprefill 8 -t 10000 -nrq 0 -nwork 8 -dist-temporary-skewed -set-count 2 -rt 1000 -ht 1000 -xi 0 0.2 -xi 1 0.3 -yi 0 0.9 -yi 1 0.8 -ti 1 900 -rti 1200
