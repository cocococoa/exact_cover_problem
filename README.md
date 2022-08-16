# Exact cover problem solver

Solve exact cover problems using dancing links

## Build

```sh
$ cmake -S . -B build/
$ cmake --build build/
```

## Apps

```sh
$ ./build/apps/example
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
$ ./build/apps/sudoku
```

## Performance

* [Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz 1.99 GHz](logs/intel-core-i7-8565U)
* [AMD Ryzen 7 3700X 8-Core Processor 3.59 GHz](logs/amd-ryzen-7-3700X)

