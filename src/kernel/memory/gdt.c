#include "memory/gdt.h"
#include "libs/stdio.h"

static gdt_entry_t gdt_entries[GDT_ENTRY_COUNT];

void gdt_set_gate(uint8_t entry_number, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[entry_number].base_low = (base & 0xFFFF);
    gdt_entries[entry_number].base_middle = (base >> 16) & 0xFF;
    gdt_entries[entry_number].base_high = (base >> 24) & 0xFF;

    gdt_entries[entry_number].limit_low = (limit & 0xFFFF);
    gdt_entries[entry_number].granularity = ((limit >> 16) & 0x0F);

    gdt_entries[entry_number].granularity |= (gran & 0xF0);
    gdt_entries[entry_number].access = access;
}

void init_tss() {
}

void init_gdt() {
    GDT_PTR.limit = sizeof(gdt_entry_t) * GDT_ENTRY_COUNT - 1;
    GDT_PTR.base = (uintptr_t)&gdt_entries;

    gdt_set_gate(GDT_ENTRY_NULL, 0, 0, 0, 0);
    gdt_set_gate(GDT_ENTRY_KERNEL_CODE, 0, 0xFFFFFFFF, 0x9A, 0xA0);
    gdt_set_gate(GDT_ENTRY_KERNEL_DATA, 0, 0xFFFFFFFF, 0x92, 0xA0);
    gdt_set_gate(GDT_ENTRY_USER_CODE, 0, 0xFFFFFFFF, 0xFA, 0xA0);
    gdt_set_gate(GDT_ENTRY_USER_DATA, 0, 0xFFFFFFFF, 0xF2, 0xA0);

     // TSS will be set up later when we have a stack and can fill in the I/O bitmap
     gdt_set_gate(GDT_ENTRY_TSS, (uintptr_t)NULL, (uintptr_t)NULL + sizeof(tss_entry_t), 0x89, 0x00);

     // Load the GDT
     __asm__ volatile ("lgdt %0" : : "m" (GDT_PTR));
}