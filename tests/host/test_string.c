#include "htest.h"
#include "libs/string.h"

HTEST(host_strlen) {
    HASSERT_EQ(strlen(""), 0u);
    HASSERT_EQ(strlen("hello"), 5u);
}

HTEST(host_strcmp_ordering) {
    HASSERT_EQ(strcmp("abc", "abc"), 0);
    HASSERT(strcmp("abc", "abd") < 0);
    HASSERT(strcmp("abd", "abc") > 0);
}

HTEST(host_strncmp_prefix) {
    HASSERT_EQ(strncmp("abcXX", "abcYY", 3), 0);
    HASSERT(strncmp("abc", "abd", 3) != 0);
}

HTEST(host_memcpy) {
    char dst[8] = {0};
    memcpy(dst, "1234567", 8);
    HASSERT_STREQ(dst, "1234567");
}

HTEST(host_memset) {
    char buf[5];
    memset(buf, 'Z', 4);
    buf[4] = '\0';
    HASSERT_STREQ(buf, "ZZZZ");
}

HTEST(host_memmove_overlap) {
    char buf[] = "abcdef";
    memmove(buf + 1, buf, 5);   // expect "aabcde"
    HASSERT_EQ(buf[0], 'a');
    HASSERT_EQ(buf[1], 'a');
    HASSERT_EQ(buf[5], 'e');
}

HTEST(host_strchr) {
    const char *s = "hello";
    HASSERT_EQ(strchr(s, 'l') - s, 2);
    HASSERT(strchr(s, 'z') == NULL);
}
