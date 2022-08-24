#!/bin/bash -eu

time ./build/apps/xc_example 2>&1     | tee -a xc_log
time ./build/apps/xc_langford 2>&1    | tee -a xc_log
time ./build/apps/xc_latin 2>&1       | tee -a xc_log
time ./build/apps/xc_nqueen 2>&1      | tee -a xc_log
time ./build/apps/xc_pentominoes 2>&1 | tee -a xc_log
time ./build/apps/xc_shikaku 2>&1     | tee -a xc_log
time ./build/apps/xc_sudoku 2>&1      | tee -a xc_log

echo "-----------------------------------------------------------" | tee -a xc_log
echo "# Summary" | tee -a xc_log
echo "" | tee -a xc_log
cat xc_log | grep Elapsed | tee -a xc_log
