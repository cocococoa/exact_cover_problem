# Exact cover problem solver

[![CMake](https://github.com/cocococoa/exact_cover_problem/actions/workflows/cmake.yml/badge.svg)](https://github.com/cocococoa/exact_cover_problem/actions/workflows/cmake.yml) [![CodeQL](https://github.com/cocococoa/exact_cover_problem/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/cocococoa/exact_cover_problem/actions/workflows/codeql-analysis.yml)

Solve exact cover problems using dancing links

## Build

```sh
$ cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release
$ cmake --build build/
```

## Test

```sh
$ ctest --test-dir build/
```

## Apps

### Exact covering (XC)

```sh
$ ./build/apps/xc_example
$ ./build/apps/xc_langford
$ ./build/apps/xc_latin
$ ./build/apps/xc_nqueen
$ ./build/apps/xc_pentominoes
$ ./build/apps/xc_shikaku
$ ./build/apps/xc_sudoku
```

### Exact covering with colors (XCC)

```sh
$ ./build/apps/xcc_example
$ ./build/apps/xcc_nqueen
$ ./build/apps/xcc_pixel_cross
$ ./build/apps/xcc_planar_langford
$ ./build/apps/xcc_prime_square
```

### Example

Cute output: 

```sh
$ ./build/apps/xc_pentominoes 
-----------------------------------------------------------
# Pentominoes

hnode size: 78
vnode size: 14981
Num copies: 36
Num solutions: 65
[Elapsed sec]  XC pentominoes: 0.0628728
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
$ ./build/apps/xc_shikaku 
-----------------------------------------------------------
# Shikaku

-------------------------------
hnode size: 62
vnode size: 239
Num copies: 3
Num solutions: 1
[Elapsed sec]                   XC shikaku 0: 0.000169849
Solution: 

         
┌───┬─┬┐ 
│   │ ││ 
├┬─┬┴─┤│ 
│├─┴┬┬┴┤ 
├┤  ││ │ 
││  ├┴─┤ 
│├──┤  │ 
└┴──┴──┘ 

-------------------------------
hnode size: 124
vnode size: 700
Num copies: 5
Num solutions: 1
[Elapsed sec]                   XC shikaku 1: 0.000380714
Solution: 

            
┌─┬┬┬────┬┐ 
├─┤││    ││ 
│ │├┴────┤│ 
│ │├────┬┴┤ 
├─┤├────┼─┤ 
├─┼┴────┴─┤ 
│ │       │ 
├─┴─────┬┬┤ 
├──┬┬──┬┤││ 
├──┤│  │├┴┤ 
└──┴┴──┴┴─┘ 

-------------------------------
hnode size: 441
vnode size: 6209
Num copies: 1
Num solutions: 1
[Elapsed sec]                   XC shikaku 2: 0.000452347
Solution: 

                      
┌───────┬┬──┬───────┐ 
│       ││  ├─┬───┬─┤ 
│       ├┴──┴─┴──┬┤ │ 
│       │        ││ │ 
│       │        │├─┤ 
│       │        │├┬┤ 
├───────┴────────┴┤││ 
│                 │├┤ 
├┬┬───────────┬───┼┤│ 
│││           │   │││ 
├┴┤           ├───┴┤│ 
├─┴──┬─┬┬┬────┴────┤│ 
│    ├─┤││         ││ 
│    ├─┴┤│         ├┤ 
│    ├─┬┴┴────┬┬───┤│ 
│    │ │      ││   ││ 
│    ├┬┴──────┴┤   ││ 
│    ││        ├┐  ││ 
├┬───┴┴────────┤│  ││ 
│├─────────────┴┴──┤│ 
└┴─────────────────┴┘
```

## Performance

* Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz 1.99 GHz
  * [xc_log](logs/intel-core-i7-8565U/xc_log)
  * [xcc_log](logs/intel-core-i7-8565U/xcc_log)
* AMD Ryzen 7 3700X 8-Core Processor 3.59 GHz
  * [xc_log](logs/amd-ryzen-7-3700X/xc_log)
  * [xcc_log](logs/amd-ryzen-7-3700X/xcc_log)
