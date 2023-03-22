cd ../microbench
rm out.txt err.txt
for key in 10000 100000 1000000 5000000
do
    prefillsize=$((key / 2))
    LD_PRELOAD=../lib/libjemalloc.so ./bin/saist_50.debra -skewed-sets -rs 0.1 -rp 0.9 -ws 0.1 -wp 0.9 -inter 0.1 -insdel 0.0 0.0 -k $key -prefillsize $prefillsize -t 10000 -nwork 1 -nprefill 1 -prefill-insert >> out.txt 2>> err.txt
done
