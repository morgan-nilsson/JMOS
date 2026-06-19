#ifndef _KTEST_H
#define _KTEST_H

#include <stdint.h>
#include <stddef.h>

// On-target (QEMU) test framework. Each KTEST() registers a case in the
// .ktests linker section; ktest_run_all() walks that section, runs every case,
// reports over serial, and signals pass/fail to the host via QEMU's
// isa-debug-exit device. See tests/kernel/ for cases and the makefile's
// `test-kernel` target for the runner wiring.

typedef struct {
    const char *name;
    const char *file;
    void (*fn)(void);
} ktest_case_t;

#define KTEST(test_name)                                                   \
    static void test_name(void);                                           \
    __attribute__((used, section(".ktests")))                              \
    static const ktest_case_t _ktest_##test_name = {                       \
        #test_name, __FILE__, test_name                                    \
    };                                                                     \
    static void test_name(void)

// Assertion primitives. On failure they flag the current case and log the
// location, but keep running (no stack unwinding in the kernel).
void ktest__assert(int cond, const char *expr, const char *file, int line);
void ktest__assert_cmp(int cond, const char *expr, uint64_t lhs, uint64_t rhs,
                       const char *file, int line);

#define KASSERT(cond) \
    ktest__assert((cond) ? 1 : 0, #cond, __FILE__, __LINE__)

#define KASSERT_EQ(a, b) \
    ktest__assert_cmp((uint64_t)(a) == (uint64_t)(b), #a " == " #b, \
                      (uint64_t)(a), (uint64_t)(b), __FILE__, __LINE__)

#define KASSERT_NE(a, b) \
    ktest__assert_cmp((uint64_t)(a) != (uint64_t)(b), #a " != " #b, \
                      (uint64_t)(a), (uint64_t)(b), __FILE__, __LINE__)

#define KASSERT_TRUE(x)     KASSERT(!!(x))
#define KASSERT_NULL(p)     KASSERT((p) == NULL)
#define KASSERT_NONNULL(p)  KASSERT((p) != NULL)

// Runs all registered cases, prints a summary, reports the result to the host,
// and halts. Never returns.
__attribute__((noreturn)) void ktest_run_all(void);

#endif
