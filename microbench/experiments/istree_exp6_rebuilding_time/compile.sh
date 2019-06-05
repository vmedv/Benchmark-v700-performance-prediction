#!/bin/sh

compile_args_for_enabled="-DMEASURE_REBUILDING_TIME"

echo "#########################################################################"
echo "#### Compiling binaries with the desired measurements enabled"
echo "#########################################################################"

here=`pwd`
mkdir ${here}/bin 2>/dev/null
cd .. ; cd .. ; make -j all bin_dir=${here}/bin_enabled xargs="$compile_args_for_enabled" > compiling.txt 2>&1
if [ "$?" -ne "0" ]; then
    echo "ERROR compiling; see compiling.txt"
    exit
fi
cd $here
