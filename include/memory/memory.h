#ifndef _memory_h
#define _memory_h

#include <limine.h>

void memory_init(struct limine_memmap_response *memmap,
                 struct limine_hhdm_response *hhdm,
                 struct limine_framebuffer *fb);

#endif
