# ensure prefetchers are enabled in a nominal state
wrmsr -a 0x1a4 0x0 > /dev/null 2>&1

# verify that huge pages are always enabled (the nominal state for linux)
hugepages_always=`cat /sys/kernel/mm/transparent_hugepage/enabled | grep -c "[always]"`
if ((hugepages_always!=1)); then
    for ((i=0;i<10;++i)); do
        echo "WARNING: HUGE PAGES ARE NOT ALWAYS ENABLED!"
    done
fi

# check for libjemalloc
jemalloc_path="../${jemalloc_file}"
if [ -e "${jemalloc_path}" ]; then
    echo "jemalloc lib found: $jemalloc_path"
else
    for ((i=0;i<10;++i)); do
        echo "WARNING: JEMALLOC LIBRARY FILE ${jemalloc_path} NOT FOUND."
    done
fi
