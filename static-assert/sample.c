#include <stdbool.h>

extern void __assert_fail(const char *desc) __attribute__ ((__noreturn__));

void _assert(bool cond, const char *desc) {
    if (!cond) {
        __assert_fail(desc);
    }
}

#define assert(expr) _assert(expr, #expr)

void test_unconditionnal() {
    assert(1 + 1 == 3);
}

void test_branch_but_unconditionnal(bool value) {
    if (value) {
        assert(1 + 1 == 3);
    } else {
        assert(1 + 2 == 2);
    }
}

void test_conditionnal(bool value) {
    if (value) {
        assert(1 + 1 == 3);
    }
}
