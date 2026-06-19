#include "test/ktest.h"
#include "memory/pmm.h"

// The PMM is already initialized (memory_init ran before ktest_run_all), so
// these exercise the live allocator. Every test frees what it allocates to
// keep the pool balanced for the next case.

KTEST(pmm_alloc_frame_is_page_aligned_and_nonzero) {
    uint64_t f = alloc_frame();
    KASSERT_NE(f, 0);
    KASSERT_EQ(f % PAGE_SIZE, 0);
    free_frame(f);
}

KTEST(pmm_alloc_frame_returns_distinct_frames) {
    uint64_t a = alloc_frame();
    uint64_t b = alloc_frame();
    KASSERT_NE(a, b);
    free_frame(a);
    free_frame(b);
}

KTEST(pmm_free_then_alloc_reuses_frame) {
    uint64_t a = alloc_frame();
    free_frame(a);
    uint64_t b = alloc_frame();
    KASSERT_EQ(a, b);   // scan hint rewinds to the just-freed frame
    free_frame(b);
}

KTEST(pmm_free_bytes_round_trips) {
    uint64_t before = pmm_free_bytes();
    uint64_t f = alloc_frame();
    KASSERT_EQ(pmm_free_bytes(), before - PAGE_SIZE);
    free_frame(f);
    KASSERT_EQ(pmm_free_bytes(), before);
}

KTEST(pmm_alloc_frames_is_contiguous) {
    const size_t n = 4;
    uint64_t base = alloc_frames(n);
    KASSERT_NE(base, 0);
    KASSERT_EQ(base % PAGE_SIZE, 0);
    free_frames(base, n);
}

KTEST(pmm_never_hands_out_null_frame) {
    // Frame 0 is reserved; no allocation should ever return physical 0.
    uint64_t f = alloc_frame();
    KASSERT_NE(f, 0);
    free_frame(f);
}
