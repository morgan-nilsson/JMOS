#include "memory/bootinfo.h"
#include "libs/string.h"

// Generous upper bound
#define MAX_REGIONS 256

uint64_t hhdm_off = 0;

static mem_region_t   regions[MAX_REGIONS];
static uint64_t       region_count = 0;
static framebuffer_t  framebuffer = { 0 };

void bootinfo_capture(struct limine_memmap_response *memmap,
                      struct limine_hhdm_response *hhdm,
                      struct limine_framebuffer *fb) {
    hhdm_off = (hhdm != NULL) ? hhdm->offset : 0;

    region_count = 0;
    if (memmap != NULL) {
        uint64_t n = memmap->entry_count;
        if (n > MAX_REGIONS) {
            n = MAX_REGIONS;
        }
        for (uint64_t i = 0; i < n; i++) {
            struct limine_memmap_entry *e = memmap->entries[i];
            regions[i].base   = e->base;
            regions[i].length = e->length;
            regions[i].type   = e->type;
        }
        region_count = n;
    }

    if (fb != NULL) {
        framebuffer.address     = (volatile uint8_t *)fb->address;
        framebuffer.width       = fb->width;
        framebuffer.height      = fb->height;
        framebuffer.pitch       = fb->pitch;
        framebuffer.bpp         = fb->bpp;
        framebuffer.red_shift   = fb->red_mask_shift;
        framebuffer.green_shift = fb->green_mask_shift;
        framebuffer.blue_shift  = fb->blue_mask_shift;
        framebuffer.present     = 1;
    } else {
        framebuffer.present = 0;
    }
}

const mem_region_t *bootinfo_regions(uint64_t *count_out) {
    if (count_out != NULL) {
        *count_out = region_count;
    }
    return regions;
}

const framebuffer_t *bootinfo_framebuffer(void) {
    return &framebuffer;
}
