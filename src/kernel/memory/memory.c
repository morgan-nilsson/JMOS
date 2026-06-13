#include "memory/memory.h"
#include "memory/bootinfo.h"
#include "memory/pmm.h"
#include "memory/paging.h"
#include "memory/kalloc.h"
#include "libs/log.h"

void memory_init(struct limine_memmap_response *memmap,
                 struct limine_hhdm_response *hhdm,
                 struct limine_framebuffer *fb) {
    LOG_HEX(LOG_TRACE, "[mem] memmap ptr =", memmap);
    LOG_HEX(LOG_TRACE, "[mem] hhdm ptr   =", hhdm);

    bootinfo_capture(memmap, hhdm, fb);

    pmm_init();
    LOG_HEX(LOG_DEBUG, "[mem] pmm free bytes =", pmm_free_bytes());

    uint64_t new_pml4 = vmm_clone_kernel_space();
    LOG_HEX(LOG_DEBUG, "[mem] new pml4 =", new_pml4);
    if (new_pml4 != 0) {
        vmm_load_space(new_pml4);
        pmm_reclaim_bootloader();
    }

    kheap_init();
}
