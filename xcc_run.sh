#!/bin/bash -eu

time ./build/apps/xcc_example 2>&1         | tee -a xcc_log
time ./build/apps/xcc_nqueen 2>&1          | tee -a xcc_log
time ./build/apps/xcc_planar_langford 2>&1 | tee -a xcc_log
time ./build/apps/xcc_prime_square 2>&1    | tee -a xcc_log

echo "-----------------------------------------------------------" | tee -a xcc_log
echo "# Summary" | tee -a xcc_log
echo "" | tee -a xcc_log
cat xcc_log | grep Elapsed | tee -a xcc_log
