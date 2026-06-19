#include "htest.h"
#include "libs/stdio.h"
#include "libs/string.h"
#include "drivers/char_device/read_write.h"

// --- Capture printf output ----------------------------------------------
// printf writes through the global `console`; stub it so the host link
// resolves and we can inspect what printf produced.

static char   g_out[4096];
static size_t g_out_len;

static void capture_write(struct char_device* dev, const char* buf, size_t count) {
    (void)dev;
    if (count > sizeof(g_out) - 1) {
        count = sizeof(g_out) - 1;
    }
    memcpy(g_out, buf, count);
    g_out[count] = '\0';
    g_out_len = count;
}

// Kernel defines this in read_write.c; provide it for the host link.
char_device_t console;

__attribute__((constructor)) static void wire_console(void) {
    console.write = capture_write;
    console.read  = NULL;
    console.data  = NULL;
}

static void reset_out(void) {
    g_out[0]  = '\0';
    g_out_len = 0;
}

// --- sprintf (already variadic-correct; baseline coverage) --------------

HTEST(sprintf_plain) {
    char buf[64];
    int n = sprintf(buf, "Hello, JMOS!\n");
    HASSERT_STREQ(buf, "Hello, JMOS!\n");
    HASSERT_EQ(n, (int)strlen("Hello, JMOS!\n"));
}

HTEST(sprintf_specifiers) {
    char buf[64];
    sprintf(buf, "d=%d x=%x u=%u s=%s c=%c", -5, 255, 42u, "hi", 'Z');
    HASSERT_STREQ(buf, "d=-5 x=ff u=42 s=hi c=Z");
}

HTEST(sprintf_percent_literal) {
    char buf[32];
    sprintf(buf, "100%%done");
    HASSERT_STREQ(buf, "100%done");
}

// --- printf (the buggy path: forwards a va_list to variadic sprintf) ----

HTEST(printf_plain_string) {
    reset_out();
    int n = printf("Hello, JMOS!\n");
    HASSERT_STREQ(g_out, "Hello, JMOS!\n");
    HASSERT_EQ(n, (int)strlen("Hello, JMOS!\n"));
}

HTEST(printf_decimal) {
    reset_out();
    printf("%d", 42);
    HASSERT_STREQ(g_out, "42");
}

HTEST(printf_negative_decimal) {
    reset_out();
    printf("%d", -7);
    HASSERT_STREQ(g_out, "-7");
}

HTEST(printf_hex) {
    reset_out();
    printf("%x", 255);
    HASSERT_STREQ(g_out, "ff");
}

HTEST(printf_unsigned) {
    reset_out();
    printf("%u", 4000000000u);
    HASSERT_STREQ(g_out, "4000000000");
}

HTEST(printf_char) {
    reset_out();
    printf("%c", 'A');
    HASSERT_STREQ(g_out, "A");
}

HTEST(printf_percent_literal) {
    reset_out();
    printf("100%%");
    HASSERT_STREQ(g_out, "100%");
}

HTEST(printf_mixed) {
    reset_out();
    printf("a=%d b=%x c=%c", 5, 255, 'Z');
    HASSERT_STREQ(g_out, "a=5 b=ff c=Z");
}

// Width/sign correctness: these would all truncate through the old itoa(int).

HTEST(printf_long_unsigned) {
    reset_out();
    printf("%lu", 10000000000UL);   // > 2^32
    HASSERT_STREQ(g_out, "10000000000");
}

HTEST(printf_long_hex) {
    reset_out();
    printf("%lx", 0xDEADBEEFUL);
    HASSERT_STREQ(g_out, "deadbeef");
}

HTEST(printf_longlong_hex_full_width) {
    reset_out();
    printf("%Lx", 0xFFFFFFFFFFFFFFFFULL);
    HASSERT_STREQ(g_out, "ffffffffffffffff");
}

HTEST(printf_long_negative) {
    reset_out();
    printf("%ld", -5L);
    HASSERT_STREQ(g_out, "-5");
}

HTEST(printf_pointer) {
    reset_out();
    printf("%p", (void*)0x1234);
    HASSERT_STREQ(g_out, "0x1234");
}
