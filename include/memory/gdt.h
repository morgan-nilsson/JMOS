#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uintptr_t base;
} __attribute__((packed)) gdt_ptr_t;

typedef struct {
    uint16_t limit;
    uintptr_t base;
} __attribute__((packed)) tss_entry_t;

static gdt_ptr_t GDT_PTR;

#define GDT_ENTRY_NULL 0
#define GDT_ENTRY_KERNEL_CODE 1
#define GDT_ENTRY_KERNEL_DATA 2
#define GDT_ENTRY_USER_CODE 3
#define GDT_ENTRY_USER_DATA 4
#define GDT_ENTRY_TSS 5

#define GDT_ENTRY_COUNT 6

#endif