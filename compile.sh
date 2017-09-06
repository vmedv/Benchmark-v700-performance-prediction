#!/bin/sh
# 
# File:   compile.sh
# Author: trbot
#
# Created on Jul 16, 2017, 8:53:32 PM
#

make -j -f microbench/Makefile &
cd macrobench
./compile.sh &
wait
