#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo
echo '###############################################'
echo '## COMPILE & TEST: microbench'
echo '###############################################'
echo

cd $DIR/../microbench_experiments/example
./run_testing.sh || exit 1
rm -r bin data

echo
echo '###############################################'
echo '## COMPILE & TEST: macrobench'
echo '###############################################'
echo

cd $DIR/../macrobench_experiments/istree_exp1
./run_testing.sh
rm -r $DIR/../macrobench/bin data
