#include <stdio.h>
#include "htest.h"

#define HTEST_MAX 512

static const char *g_names[HTEST_MAX];
static htest_fn    g_fns[HTEST_MAX];
static int         g_count;

int htest_current_failed;

void htest_register(const char *name, htest_fn fn) {
    if (g_count < HTEST_MAX) {
        g_names[g_count] = name;
        g_fns[g_count]   = fn;
        g_count++;
    }
}

void htest_fail(const char *expr, const char *file, int line) {
    htest_current_failed = 1;
    fprintf(stdout, "    FAIL: %s (%s:%d)\n", expr, file, line);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);   // flush each line so a crash shows progress

    int passed = 0;
    int failed = 0;

    fprintf(stdout, "==== host tests: %d registered ====\n", g_count);
    for (int i = 0; i < g_count; i++) {
        htest_current_failed = 0;
        g_fns[i]();
        if (htest_current_failed) {
            failed++;
            fprintf(stdout, "[ FAIL ] %s\n", g_names[i]);
        } else {
            passed++;
            fprintf(stdout, "[  OK  ] %s\n", g_names[i]);
        }
    }
    fprintf(stdout, "==== %d passed, %d failed ====\n", passed, failed);
    return failed ? 1 : 0;
}
