# `static-assert`

A small pass I made when discussing on IRC (`#rust-fr` on mozilla IRC server) about detecting at compile-time failed
assert when the compiler could optimize a `assert(EXPR)` to a simple `assert(false)`

## Options

- `-assert-function-names`:
    Define the function names to detect when searching failed asserts.
    You can pass this option multiple times to define multiple functions.
    (type: list of strings)

## Example

Run this in the build directory:
```
$ clang -O3 -S -emit-llvm -o sample.ll ../static-assert/sample.c
$ opt -load lib/LLVMStaticAssert.so -static-assert -assert-function-names=__assert_fail sample.ll 2>&1 >/dev/null
Warning: The function `test_unconditionnal` unconditionally fails with:
  tail call void @__assert_fail(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str, i64 0, i64 0)) #2
Warning: The function `test_branch_but_unconditionnal` unconditionally fails with:
  tail call void @__assert_fail(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str, i64 0, i64 0)) #2
  tail call void @__assert_fail(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.1, i64 0, i64 0)) #2
```

## TODO

- [ ] Print more informations about the failed assert (like using debug informations).
- [ ] Add the ability to use this pass as a linkable library.
- [ ] Maybe play with the [Rust](https://www.rust-lang.org) compiler to integrate this pass in the
      compile process.
