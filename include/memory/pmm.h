#ifndef _pmm_h
#define _pmm_h

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096ull

void pmm_init(void);

uint64_t alloc_frame(void);

uint64_t alloc_frames(size_t count);

void free_frame(uint64_t phys);
void free_frames(uint64_t phys, size_t count);

void pmm_reclaim_bootloader(void);

uint64_t pmm_total_bytes(void);
uint64_t pmm_free_bytes(void);

#endif
