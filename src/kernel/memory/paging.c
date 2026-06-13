#include <stdint.h>
#include <stddef.h>
#include "memory/paging.h"
#include "memory/pmm.h"
#include "libs/stdlib.h"
#include "libs/string.h"

#define PAGE_PRESENT (1ull << 0)
#define PAGE_WRITE   (1ull << 1)
#define PAGE_USER    (1ull << 2)
#define PAGE_HUGE    (1ull << 7)   // 2MiB at PD level, 1GiB at PDPT level
#define PAGE_NX      (1ull << 63)
#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000ull   // bits 51:12

#define PML4_IDX(v) (((v) >> 39) & 0x1FF)   // bits 47:39
#define PDPT_IDX(v) (((v) >> 30) & 0x1FF)   // bits 38:30
#define PD_IDX(v)   (((v) >> 21) & 0x1FF)   // bits 29:21
#define PT_IDX(v)   (((v) >> 12) & 0x1FF)   // bits 20:12

extern uint64_t hhdm_off;
static inline uint64_t *p2v(uint64_t phys) { 
    return (uint64_t *)(phys + hhdm_off); 
}

static uint64_t *next_level(uint64_t *tbl, size_t i) {
    if (!(tbl[i] & PAGE_PRESENT)) {
        uint64_t frame = alloc_frame();
        if (frame == 0) {
            return NULL; // out of memory
        }
        memset(p2v(frame), 0, 4096);
        tbl[i] = frame | PAGE_PRESENT | PAGE_WRITE;
    }
    return p2v(tbl[i] & PAGE_ADDR_MASK);
}

void map_page(uint64_t *pml4, virtual_address_t va, physical_address_t pa, uint64_t flags) {
    uint64_t *pdpt = next_level(pml4, PML4_IDX(va));
    if (pdpt == NULL) return;

    uint64_t *pd = next_level(pdpt, PDPT_IDX(va));
    if (pd == NULL) return;

    uint64_t *pt = next_level(pd, PD_IDX(va));
    if (pt == NULL) return;

    pt[PT_IDX(va)] = (pa & PAGE_ADDR_MASK) | (flags & 0xFFF) | (flags & PAGE_NX);
    __asm__ volatile ("invlpg [%0]" : : "r" (va) : "memory");
}

static uint64_t clone_table(uint64_t *src, int level) {
    uint64_t frame = alloc_frame();
    if (frame == 0) {
        return 0;
    }
    uint64_t *dst = p2v(frame);

    for (size_t i = 0; i < 512; i++) {
        uint64_t e = src[i];
        if (!(e & PAGE_PRESENT)) {
            dst[i] = 0;
            continue;
        }

        int leaf = (level == 1) || ((level == 2 || level == 3) && (e & PAGE_HUGE));
        if (leaf) {
            dst[i] = e;
            continue;
        }

        uint64_t child = clone_table(p2v(e & PAGE_ADDR_MASK), level - 1);
        if (child == 0) {
            return 0;
        }
        // Keep the original flag bits, swap in the relocated child address.
        dst[i] = child | (e & ~PAGE_ADDR_MASK);
    }
    return frame;
}

uint64_t vmm_clone_kernel_space(void) {
    uint64_t cr3;
    __asm__ volatile ("mov %0, cr3" : "=r" (cr3));
    return clone_table(p2v(cr3 & PAGE_ADDR_MASK), 4);
}

void vmm_load_space(physical_address_t pml4_phys) {
    __asm__ volatile ("mov cr3, %0" : : "r" (pml4_phys) : "memory");
}