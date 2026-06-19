#include "test/ktest.h"
#include "libs/string.h"

KTEST(string_strlen) {
    KASSERT_EQ(strlen(""), 0);
    KASSERT_EQ(strlen("hello"), 5);
}

KTEST(string_strcmp_ordering) {
    KASSERT_EQ(strcmp("abc", "abc"), 0);
    KASSERT_TRUE(strcmp("abc", "abd") < 0);
    KASSERT_TRUE(strcmp("abd", "abc") > 0);
}

KTEST(string_memcpy_copies_bytes) {
    char dst[8] = {0};
    memcpy(dst, "1234567", 8);
    KASSERT_EQ(strcmp(dst, "1234567"), 0);
}

KTEST(string_memset_fills) {
    char buf[5];
    memset(buf, 'A', 4);
    buf[4] = '\0';
    KASSERT_EQ(strcmp(buf, "AAAA"), 0);
}

KTEST(string_memmove_handles_overlap) {
    char buf[] = "abcdef";
    memmove(buf + 1, buf, 5);   // expect "aabcde"
    KASSERT_EQ(buf[0], 'a');
    KASSERT_EQ(buf[1], 'a');
    KASSERT_EQ(buf[2], 'b');
    KASSERT_EQ(buf[5], 'e');
}
