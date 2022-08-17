#!/bin/bash -eu

time ./build/apps/langford 2>&1     | tee -a log
time ./build/apps/latin 2>&1        | tee -a log
time ./build/apps/pentominoes 2>&1  | tee -a log
time ./build/apps/prime_square 2>&1 | tee -a log
time ./build/apps/sudoku 2>&1       | tee -a log
time ./build/apps/xc_example 2>&1   | tee -a log
time ./build/apps/xc_nqueen 2>&1    | tee -a log
time ./build/apps/xcc_example 2>&1  | tee -a log
time ./build/apps/xcc_nqueen 2>&1   | tee -a log

echo "-----------------------------------------------------------" | tee -a log
echo "# Summary" | tee -a log
echo "" | tee -a log
cat log | grep Elapsed | tee -a log
