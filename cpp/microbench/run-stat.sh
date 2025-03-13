#!/usr/bin/env bash

IT_N=$2 DS=$1 bash -c 'LIBPFM_FORCE_PMU=amd64 LD_PRELOAD=../lib/libmimalloc.so ./bin/${DS}.debra -create-default-prefill -json-file cache-measurements/conf.json'
