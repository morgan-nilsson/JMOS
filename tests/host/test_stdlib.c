#include "htest.h"
#include "libs/stdlib.h"
#include "libs/string.h"
#include <limits.h>

// ---- atoi ---------------------------------------------------------------

HTEST(atoi_basic) {
    HASSERT_EQ(atoi("123"), 123);
    HASSERT_EQ(atoi("0"), 0);
    HASSERT_EQ(atoi("00042"), 42);
}

HTEST(atoi_signs) {
    HASSERT_EQ(atoi("-42"), -42);
    HASSERT_EQ(atoi("+7"), 7);
    HASSERT_EQ(atoi("-0"), 0);
}

HTEST(atoi_leading_whitespace) {
    HASSERT_EQ(atoi("   \t\n42"), 42);
}

HTEST(atoi_stops_at_non_digit) {
    HASSERT_EQ(atoi("9x9"), 9);
    HASSERT_EQ(atoi("12 34"), 12);
}

HTEST(atoi_no_digits_is_zero) {
    HASSERT_EQ(atoi(""), 0);
    HASSERT_EQ(atoi("abc"), 0);
    HASSERT_EQ(atoi("+"), 0);
}

// ---- atol / atoll (exercise widths beyond 32 bits) ----------------------

HTEST(atol_wide) {
    HASSERT_EQ(atol("2147483648"), 2147483648L);     // INT_MAX + 1
    HASSERT_EQ(atol("-2147483648"), -2147483648L);
}

HTEST(atoll_wide) {
    HASSERT_EQ(atoll("9223372036854775807"), 9223372036854775807LL);  // LLONG_MAX
    HASSERT_EQ(atoll("-9000000000000000000"), -9000000000000000000LL);
}

// ---- strtol -------------------------------------------------------------

HTEST(strtol_base10) {
    HASSERT_EQ(strtol("42", NULL, 10), 42L);
    HASSERT_EQ(strtol("-42", NULL, 10), -42L);
    HASSERT_EQ(strtol("+42", NULL, 10), 42L);
    HASSERT_EQ(strtol("   10", NULL, 10), 10L);
}

HTEST(strtol_base16) {
    HASSERT_EQ(strtol("1A", NULL, 16), 26L);
    HASSERT_EQ(strtol("0x1A", NULL, 16), 26L);     // 0x prefix tolerated
    HASSERT_EQ(strtol("DEAD", NULL, 16), 57005L);  // uppercase
    HASSERT_EQ(strtol("dead", NULL, 16), 57005L);  // lowercase
}

HTEST(strtol_base0_autodetect) {
    HASSERT_EQ(strtol("0x1A", NULL, 0), 26L);   // hex
    HASSERT_EQ(strtol("010", NULL, 0), 8L);     // octal
    HASSERT_EQ(strtol("42", NULL, 0), 42L);     // decimal
    HASSERT_EQ(strtol("0", NULL, 0), 0L);
}

HTEST(strtol_other_bases) {
    HASSERT_EQ(strtol("101", NULL, 2), 5L);
    HASSERT_EQ(strtol("777", NULL, 8), 511L);
    HASSERT_EQ(strtol("z", NULL, 36), 35L);
    HASSERT_EQ(strtol("zz", NULL, 36), 36L * 35 + 35);
}

HTEST(strtol_endptr) {
    char *end;
    long v = strtol("42abc", &end, 10);
    HASSERT_EQ(v, 42L);
    HASSERT_STREQ(end, "abc");

    v = strtol("ff!", &end, 16);
    HASSERT_EQ(v, 255L);
    HASSERT_STREQ(end, "!");
}

HTEST(strtol_digit_out_of_base_stops) {
    char *end;
    long v = strtol("129", &end, 8);   // '9' is not an octal digit
    HASSERT_EQ(v, 012L);               // parses "12" as octal = 10
    HASSERT_STREQ(end, "9");
}

// ---- strtoll ------------------------------------------------------------

HTEST(strtoll_wide) {
    HASSERT_EQ(strtoll("9223372036854775807", NULL, 10), 9223372036854775807LL);
    HASSERT_EQ(strtoll("-1", NULL, 10), -1LL);
    HASSERT_EQ(strtoll("7fffffffffffffff", NULL, 16), 9223372036854775807LL);
}

// ---- strtoul / strtoull -------------------------------------------------

HTEST(strtoul_basic) {
    HASSERT_EQ(strtoul("255", NULL, 10), 255UL);
    HASSERT_EQ(strtoul("ff", NULL, 16), 255UL);
    HASSERT_EQ(strtoul("0xFF", NULL, 0), 255UL);
    HASSERT_EQ(strtoul("4294967295", NULL, 10), 4294967295UL);
}

HTEST(strtoul_endptr) {
    char *end;
    unsigned long v = strtoul("100xyz", &end, 10);
    HASSERT_EQ(v, 100UL);
    HASSERT_STREQ(end, "xyz");
}

HTEST(strtoull_wide) {
    HASSERT_EQ(strtoull("18446744073709551615", NULL, 10), 18446744073709551615ULL);
    HASSERT_EQ(strtoull("ffffffffffffffff", NULL, 16), 18446744073709551615ULL);
}

// Per C standard, strtoul accepts a leading '-' and returns the negated value
// (mod 2^N). Documents conformance.
HTEST(strtoul_negative_wraps_per_standard) {
    HASSERT_EQ(strtoul("-1", NULL, 10), ULONG_MAX);
}

// ---- itoa ---------------------------------------------------------------

HTEST(itoa_base10) {
    char buf[32];
    HASSERT_STREQ(itoa(123, buf, 10), "123");
    HASSERT_STREQ(itoa(-123, buf, 10), "-123");
    HASSERT_STREQ(itoa(0, buf, 10), "0");
}

HTEST(itoa_other_bases_positive) {
    char buf[40];
    HASSERT_STREQ(itoa(255, buf, 16), "ff");
    HASSERT_STREQ(itoa(10, buf, 2), "1010");
    HASSERT_STREQ(itoa(35, buf, 36), "z");
}

HTEST(itoa_invalid_base_yields_empty) {
    char buf[8];
    HASSERT_STREQ(itoa(5, buf, 1), "");
    HASSERT_STREQ(itoa(5, buf, 37), "");
}

// Property check: every emitted character must be a valid digit for the base
// (optionally a leading '-'). Catches negative values in non-decimal bases.
static int all_valid_digits(const char *s, int base) {
    if (*s == '-') {
        s++;
    }
    if (*s == '\0') {
        return 0;   // must emit at least one digit
    }
    for (; *s; s++) {
        int d;
        if (*s >= '0' && *s <= '9')      d = *s - '0';
        else if (*s >= 'a' && *s <= 'z') d = *s - 'a' + 10;
        else                             return 0;
        if (d >= base) {
            return 0;
        }
    }
    return 1;
}

HTEST(itoa_negative_nondecimal_emits_valid_digits) {
    char buf[40];
    itoa(-255, buf, 16);
    HASSERT_TRUE(all_valid_digits(buf, 16));
}

// ---- abs family ---------------------------------------------------------

HTEST(abs_values) {
    HASSERT_EQ(abs(-5), 5);
    HASSERT_EQ(abs(5), 5);
    HASSERT_EQ(abs(0), 0);
}

HTEST(labs_values) {
    HASSERT_EQ(labs(-123456789L), 123456789L);
    HASSERT_EQ(labs(123456789L), 123456789L);
}

HTEST(llabs_values) {
    HASSERT_EQ(llabs(-9000000000000LL), 9000000000000LL);
    HASSERT_EQ(llabs(9000000000000LL), 9000000000000LL);
}
