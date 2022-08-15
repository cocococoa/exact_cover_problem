#!/bin/bash -eu

echo "-----------------------------------------------------------"
echo "# Example"
echo ""
time ./build/apps/example

echo "-----------------------------------------------------------"
echo "# Langford pair"
echo ""
time ./build/apps/langford

echo "-----------------------------------------------------------"
echo "# Latin square"
echo ""
time ./build/apps/latin

echo "-----------------------------------------------------------"
echo "# N Queen"
echo ""
time ./build/apps/nqueen

echo "-----------------------------------------------------------"
echo "# Pentominoes"
echo ""
time ./build/apps/pentominoes

echo "-----------------------------------------------------------"
echo "# Sudoku"
echo ""
time ./build/apps/sudoku

