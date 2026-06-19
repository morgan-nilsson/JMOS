#include "test/ktest.h"
#include "libs/serial.h"
#include "libs/stdio.h"   // outb

// Provided by the linker (.ktests section bounds, see build_tools/kernel.ld).
extern const ktest_case_t __start_ktests[];
extern const ktest_case_t __stop_ktests[];

static int      current_failed;
static uint64_t asserts_total;
static uint64_t asserts_failed;

// QEMU isa-debug-exit: writing `v` to port 0xF4 exits QEMU with status
// (v << 1) | 1. The makefile maps exit code 1 (v=0) to PASS, 3 (v=1) to FAIL.
static void qemu_exit(uint8_t v) {
    outb(0xF4, v);
}

void ktest__assert(int cond, const char *expr, const char *file, int line) {
    asserts_total++;
    if (cond) {
        return;
    }
    asserts_failed++;
    current_failed = 1;
    serial_puts("    FAIL: ");
    serial_puts(expr);
    serial_puts(" (");
    serial_puts(file);
    serial_putc(':');
    serial_put_dec((uint64_t)line);
    serial_puts(")\n");
}

void ktest__assert_cmp(int cond, const char *expr, uint64_t lhs, uint64_t rhs,
                       const char *file, int line) {
    asserts_total++;
    if (cond) {
        return;
    }
    asserts_failed++;
    current_failed = 1;
    serial_puts("    FAIL: ");
    serial_puts(expr);
    serial_puts("  [lhs=");
    serial_put_hex(lhs);
    serial_puts(" rhs=");
    serial_put_hex(rhs);
    serial_puts("] (");
    serial_puts(file);
    serial_putc(':');
    serial_put_dec((uint64_t)line);
    serial_puts(")\n");
}

void ktest_run_all(void) {
    const uint64_t n = (uint64_t)(__stop_ktests - __start_ktests);
    uint64_t passed = 0;
    uint64_t failed = 0;

    serial_puts("\n==== ktest: running ");
    serial_put_dec(n);
    serial_puts(" cases ====\n");

    for (const ktest_case_t *t = __start_ktests; t < __stop_ktests; t++) {
        current_failed = 0;
        serial_puts("[ RUN  ] ");
        serial_puts(t->name);
        serial_putc('\n');

        t->fn();

        if (current_failed) {
            failed++;
            serial_puts("[ FAIL ] ");
        } else {
            passed++;
            serial_puts("[  OK  ] ");
        }
        serial_puts(t->name);
        serial_putc('\n');
    }

    serial_puts("==== ktest: ");
    serial_put_dec(passed);
    serial_puts(" passed, ");
    serial_put_dec(failed);
    serial_puts(" failed; ");
    serial_put_dec(asserts_failed);
    serial_putc('/');
    serial_put_dec(asserts_total);
    serial_puts(" assertions failed ====\n");

    qemu_exit(failed == 0 ? 0 : 1);

    // qemu_exit terminates the VM; if it somehow returns, wedge deliberately.
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
