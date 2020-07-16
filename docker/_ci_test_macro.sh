#!/bin/bash

if [ "$#" -ne "1" ]; then
    echo "USAGE: $(basename $0) USER/REPO:TAG"
    exit 1
fi
uri=$1

docker container rm setbench_test >/dev/null 2>&1
failure=1

docker run --name setbench_test --privileged $uri /root/setbench/docker/_run_test_in_container.sh
if [ "$?" -eq "0" ]; then
    echo
    echo "SUCCESS."
    failure=0
fi
docker container rm setbench_test >/dev/null 2>&1
exit $failure
