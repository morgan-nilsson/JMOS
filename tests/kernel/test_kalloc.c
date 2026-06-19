#include "test/ktest.h"
#include "memory/kalloc.h"
#include "libs/string.h"

KTEST(kmalloc_returns_writable_memory) {
    uint8_t *p = kmalloc(128);
    KASSERT_NONNULL(p);
    memset(p, 0xAB, 128);
    KASSERT_EQ(p[0], 0xAB);
    KASSERT_EQ(p[127], 0xAB);
    kfree(p);
}

KTEST(kmalloc_zero_returns_null) {
    KASSERT_NULL(kmalloc(0));
}

KTEST(kmalloc_is_16_byte_aligned) {
    void *p = kmalloc(1);
    KASSERT_NONNULL(p);
    KASSERT_EQ((uint64_t)p % 16, 0);
    kfree(p);
}

KTEST(kcalloc_zeroes_memory) {
    uint8_t *p = kcalloc(64, 1);
    KASSERT_NONNULL(p);
    int all_zero = 1;
    for (int i = 0; i < 64; i++) {
        if (p[i] != 0) {
            all_zero = 0;
        }
    }
    KASSERT_TRUE(all_zero);
    kfree(p);
}

KTEST(kcalloc_overflow_returns_null) {
    KASSERT_NULL(kcalloc((size_t)-1, 2));
}

KTEST(krealloc_grows_and_preserves_contents) {
    uint8_t *p = kmalloc(16);
    KASSERT_NONNULL(p);
    for (int i = 0; i < 16; i++) {
        p[i] = (uint8_t)i;
    }
    uint8_t *q = krealloc(p, 64);
    KASSERT_NONNULL(q);
    int preserved = 1;
    for (int i = 0; i < 16; i++) {
        if (q[i] != (uint8_t)i) {
            preserved = 0;
        }
    }
    KASSERT_TRUE(preserved);
    kfree(q);
}

KTEST(kfree_null_is_noop) {
    kfree(NULL);   // must not fault
    KASSERT_TRUE(1);
}

KTEST(kmalloc_distinct_allocations_dont_overlap) {
    uint8_t *a = kmalloc(32);
    uint8_t *b = kmalloc(32);
    KASSERT_NONNULL(a);
    KASSERT_NONNULL(b);
    KASSERT_NE(a, b);
    // Writing through one must not disturb the other.
    memset(a, 0x11, 32);
    memset(b, 0x22, 32);
    KASSERT_EQ(a[0], 0x11);
    KASSERT_EQ(b[0], 0x22);
    kfree(a);
    kfree(b);
}
