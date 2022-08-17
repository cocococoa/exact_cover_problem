#!/bin/bash -eu

time ./build/apps/example 2>&1     | tee -a log
time ./build/apps/langford 2>&1    | tee -a log
time ./build/apps/latin 2>&1       | tee -a log
time ./build/apps/nqueen 2>&1      | tee -a log
time ./build/apps/pentominoes 2>&1 | tee -a log
time ./build/apps/sudoku 2>&1      | tee -a log

echo "-----------------------------------------------------------"
echo "# Summary"
echo ""
cat log | grep Elapsed             | tee -a log
