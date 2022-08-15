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
```
