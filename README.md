# KokaKiwi's LLVM passes

Some LLVM passes made by myself during my plays with LLVM, some are "serious", some are just PoCs,
some are just fun things I wanted to do when playing with LLVM.

## Requirements

Just some obvious requirements, I think :)

- CMake
- A C++ compiler
- The [LLVM](http://llvm.org) library installed on the system (version 3.7)

## How to build

Simply follow basic steps for a CMake project:
```
mkdir build && cd build
cmake ..
make
```

## Passes

- `static-assert`:
    A small pass I made when discussing on IRC about detecting at compile-time failed assert when the
    compiler could reducing a `assert(EXPR)` to a simple `assert(false)`
