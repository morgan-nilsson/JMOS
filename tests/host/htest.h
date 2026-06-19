#ifndef _HTEST_H
#define _HTEST_H

#include <stddef.h>   // size_t, NULL (host <stdio.h> is intentionally avoided)

// NB: deliberately does NOT include host <stdio.h> — some units under test
// (libs/stdio.h) declare names like snprintf that clash with libc's fortify
// macros. Only runner.c needs host stdio.

// Host (tier 1) unit-test framework. Runs natively on the dev machine against
// the freestanding-pure kernel libs (src/libs/string.c, stdlib.c). Registration
// is via __attribute__((constructor)) because Mach-O has no .ktests-style
// section encapsulation; otherwise the ergonomics mirror ktest.h.

typedef void (*htest_fn)(void);

void htest_register(const char *name, htest_fn fn);
void htest_fail(const char *expr, const char *file, int line);
extern int htest_current_failed;

#define HTEST(name)                                                     \
    static void name(void);                                             \
    __attribute__((constructor)) static void htest_reg_##name(void) {   \
        htest_register(#name, name);                                    \
    }                                                                   \
    static void name(void)

#define HASSERT(cond) \
    do { if (!(cond)) htest_fail(#cond, __FILE__, __LINE__); } while (0)

#define HASSERT_EQ(a, b)    HASSERT((a) == (b))
#define HASSERT_NE(a, b)    HASSERT((a) != (b))
#define HASSERT_TRUE(x)     HASSERT(!!(x))
#define HASSERT_STREQ(a, b) HASSERT(strcmp((a), (b)) == 0)

#endif
