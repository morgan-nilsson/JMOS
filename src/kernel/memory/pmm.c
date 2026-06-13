#include "memory/pmm.h"
#include "memory/bootinfo.h"
#include "libs/string.h"
#include "libs/log.h"
#include <limine.h>

static uint8_t  *bitmap;        // HHDM pointer to the bitmap bytes
static uint64_t  frame_count;   // total frames the bitmap spans
static uint64_t  bitmap_bytes;  // size of the bitmap in bytes
static uint64_t  free_frames_n; // currently free frames
static uint64_t  total_frames;  // frames under management (usable + reclaimed)
static uint64_t  scan_hint;     // index to start the next allocation scan from

static inline void bm_set(uint64_t i)   { bitmap[i >> 3] |=  (uint8_t)(1u << (i & 7)); }
static inline void bm_clear(uint64_t i)  { bitmap[i >> 3] &= (uint8_t)~(1u << (i & 7)); }
static inline int  bm_test(uint64_t i)   { return (bitmap[i >> 3] >> (i & 7)) & 1; }

static inline uint64_t align_up(uint64_t v, uint64_t a)   { return (v + a - 1) & ~(a - 1); }
static inline uint64_t align_down(uint64_t v, uint64_t a) { return v & ~(a - 1); }

// Mark every frame fully contained in [base, base+length) free.
static void free_region(uint64_t base, uint64_t length) {
    uint64_t start = align_up(base, PAGE_SIZE) / PAGE_SIZE;
    uint64_t end   = align_down(base + length, PAGE_SIZE) / PAGE_SIZE;
    for (uint64_t i = start; i < end && i < frame_count; i++) {
        if (bm_test(i)) {
            bm_clear(i);
            free_frames_n++;
        }
    }
}

void pmm_init(void) {
    uint64_t count;
    const mem_region_t *regions = bootinfo_regions(&count);

    LOG_AT(LOG_TRACE,
        for (uint64_t i = 0; i < count; i++) {
            serial_puts("[pmm] region base="); serial_put_hex(regions[i].base);
            serial_puts(" len=");              serial_put_hex(regions[i].length);
            serial_puts(" type=");             serial_put_dec(regions[i].type);
            serial_putc('\n');
        }
    );

    uint64_t highest = 0;
    for (uint64_t i = 0; i < count; i++) {
        uint64_t end = regions[i].base + regions[i].length;
        if ((regions[i].type == LIMINE_MEMMAP_USABLE ||
             regions[i].type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) &&
            end > highest) {
            highest = end;
        }
    }

    frame_count  = align_up(highest, PAGE_SIZE) / PAGE_SIZE;
    bitmap_bytes = align_up((frame_count + 7) / 8, PAGE_SIZE);

    LOG_DEC(LOG_DEBUG, "[pmm] frame_count =", frame_count);

    // 0 is a valid physical address (conventional memory starts there), so it
    // can't double as a "not found" sentinel — track success explicitly.
    uint64_t bitmap_phys = 0;
    int bitmap_found = 0;
    for (uint64_t i = 0; i < count; i++) {
        if (regions[i].type != LIMINE_MEMMAP_USABLE) {
            continue;
        }
        uint64_t base = align_up(regions[i].base, PAGE_SIZE);
        uint64_t avail = regions[i].base + regions[i].length - base;
        if (avail >= bitmap_bytes) {
            bitmap_phys = base;
            bitmap_found = 1;
            break;
        }
    }
    LOG_HEX(LOG_TRACE, "[pmm] bitmap_phys =", bitmap_phys);
    // No region big enough means something is very wrong; nothing else to do.
    if (!bitmap_found) {
        return;
    }
    bitmap = (uint8_t *)(bitmap_phys + hhdm_off);

    // Start with everything marked used, then free the usable regions.
    memset(bitmap, 0xFF, bitmap_bytes);
    free_frames_n = 0;
    for (uint64_t i = 0; i < count; i++) {
        if (regions[i].type == LIMINE_MEMMAP_USABLE) {
            free_region(regions[i].base, regions[i].length);
        }
    }
    total_frames = free_frames_n;
    LOG_DEC(LOG_DEBUG, "[pmm] free frames =", free_frames_n);

    // Carve the bitmap's own storage back out of the free pool.
    uint64_t bm_start = bitmap_phys / PAGE_SIZE;
    uint64_t bm_end   = bm_start + bitmap_bytes / PAGE_SIZE;
    for (uint64_t i = bm_start; i < bm_end; i++) {
        if (!bm_test(i)) {
            bm_set(i);
            free_frames_n--;
        }
    }

    // Never hand out the null frame.
    if (frame_count > 0 && !bm_test(0)) {
        bm_set(0);
        free_frames_n--;
    }

    scan_hint = 0;
}

uint64_t alloc_frame(void) {
    for (uint64_t pass = 0; pass < 2; pass++) {
        uint64_t start = (pass == 0) ? scan_hint : 0;
        uint64_t stop  = (pass == 0) ? frame_count : scan_hint;
        for (uint64_t i = start; i < stop; i++) {
            if (!bm_test(i)) {
                bm_set(i);
                free_frames_n--;
                scan_hint = i + 1;
                return i * PAGE_SIZE;
            }
        }
    }
    return 0;
}

uint64_t alloc_frames(size_t count) {
    if (count == 0) {
        return 0;
    }
    if (count == 1) {
        return alloc_frame();
    }

    uint64_t run = 0;
    uint64_t run_start = 0;
    for (uint64_t i = 0; i < frame_count; i++) {
        if (!bm_test(i)) {
            if (run == 0) {
                run_start = i;
            }
            if (++run == count) {
                for (uint64_t j = run_start; j < run_start + count; j++) {
                    bm_set(j);
                }
                free_frames_n -= count;
                return run_start * PAGE_SIZE;
            }
        } else {
            run = 0;
        }
    }
    return 0;
}

void free_frame(uint64_t phys) {
    uint64_t i = phys / PAGE_SIZE;
    if (i == 0 || i >= frame_count) {
        return;
    }
    if (bm_test(i)) {
        bm_clear(i);
        free_frames_n++;
        if (i < scan_hint) {
            scan_hint = i;
        }
    }
}

void free_frames(uint64_t phys, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free_frame(phys + i * PAGE_SIZE);
    }
}

void pmm_reclaim_bootloader(void) {
    uint64_t count;
    const mem_region_t *regions = bootinfo_regions(&count);
    uint64_t before = free_frames_n;
    for (uint64_t i = 0; i < count; i++) {
        if (regions[i].type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            free_region(regions[i].base, regions[i].length);
        }
    }
    // Guard the null frame in case a reclaimable region started at 0.
    if (frame_count > 0 && !bm_test(0)) {
        bm_set(0);
        free_frames_n--;
    }
    total_frames += free_frames_n - before;
    scan_hint = 0;
}

uint64_t pmm_total_bytes(void) { return total_frames * PAGE_SIZE; }
uint64_t pmm_free_bytes(void)  { return free_frames_n * PAGE_SIZE; }
