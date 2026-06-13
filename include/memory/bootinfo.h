#ifndef _bootinfo_h
#define _bootinfo_h

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

extern uint64_t hhdm_off;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint64_t type;
} mem_region_t;

typedef struct {
    volatile uint8_t *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t  red_shift;
    uint8_t  green_shift;
    uint8_t  blue_shift;
    int      present;
} framebuffer_t;

void bootinfo_capture(struct limine_memmap_response *memmap,
                      struct limine_hhdm_response *hhdm,
                      struct limine_framebuffer *fb);

const mem_region_t *bootinfo_regions(uint64_t *count_out);

const framebuffer_t *bootinfo_framebuffer(void);

#endif
