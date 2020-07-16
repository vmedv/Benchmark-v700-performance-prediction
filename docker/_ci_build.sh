#!/bin/bash

if [ "$#" -ne "1" ]; then
    echo "USAGE: $(basename $0) USER/REPO:TAG"
    exit 1
fi
uri=$1

## build a new docker image from current git state
docker build -f Dockerfile ../ -t $uri
