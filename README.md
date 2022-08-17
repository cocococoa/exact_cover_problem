# Exact cover problem solver

[![CMake](https://github.com/cocococoa/exact_cover_problem/actions/workflows/cmake.yml/badge.svg)](https://github.com/cocococoa/exact_cover_problem/actions/workflows/cmake.yml) [![CodeQL](https://github.com/cocococoa/exact_cover_problem/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/cocococoa/exact_cover_problem/actions/workflows/codeql-analysis.yml)

Solve exact cover problems using dancing links

## Build

```sh
$ cmake -S . -B build/
$ cmake --build build/
```

## Apps

```sh
$ ./build/apps/langford
$ ./build/apps/latin
$ ./build/apps/nqueen
$ ./build/apps/pentominoes
hnode size: 78
vnode size: 14981
Find exact cover via dancing links
Num copies: 36
Done
Num solutions: 65
Elapsed: 0.13393 [seconds]
-------------------------------
Solution: 0
          
┌┬┬──┬──┐ 
││└┬┐├┐ │ 
││┌┘││└─┤ 
│││┌┴┼─┐│ 
│├┤│ ├┐└┤ 
├┘└┼┬┘└┐│ 
├┐┌┤└─┐├┤ 
│└┘│┌─┴┘│ 
└──┴┴───┘ 
-------------------------------
Solution: 10
          
┌──┬───┬┐ 
├─┐├┐┌─┘│ 
│┌┤│└┴┐┌┤ 
││└┼─┐├┘│ 
││ │ ├┘┌┤ 
├┴┬┼─┼─┘│ 
│┌┘└┐└─┐│ 
│└┐┌┴──┴┤ 
└─┴┴────┘
$ ./build/apps/prime_square
$ ./build/apps/sudoku
$ ./build/apps/xc_example
$ ./build/apps/xcc_example
```

## Performance

* [Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz 1.99 GHz](logs/intel-core-i7-8565U)
* [AMD Ryzen 7 3700X 8-Core Processor 3.59 GHz](logs/amd-ryzen-7-3700X)

