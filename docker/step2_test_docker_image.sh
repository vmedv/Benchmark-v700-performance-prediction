#!/bin/bash

failure=1

docker run --name setbench_test -it --privileged setbench /root/setbench/docker/_run_test_in_container.sh
if [ "$?" -eq "0" ]; then
    echo
    echo "SUCCESS."
    failure=0
fi
docker container rm setbench_test >/dev/null 2>&1

exit $failure
