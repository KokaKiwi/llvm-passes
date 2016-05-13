#!/bin/bash
HERE=$(realpath $(dirname $0))

# Assumption: This script is running in the build directory.
mkdir -p static-assert
clang -O3 -S -emit-llvm -o static-assert/sample.ll ${HERE}/sample.c
opt -load lib/LLVMStaticAssert.so -static-assert -assert-function-names=__assert_fail static-assert/sample.ll 2>&1 >/dev/null
