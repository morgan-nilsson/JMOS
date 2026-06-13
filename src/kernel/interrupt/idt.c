#include "interrupt/idt.h"

idt_gate_descriptor_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate_descriptor(uint8_t n, uint64_t address, uint16_t ss, uint8_t ist, uint16_t flags) {
    idt[n].offset_1 = address & 0xFFFF;
    idt[n].segment_selector = ss;
    idt[n].ist = ist & 0x7;
    idt[n]._reserved = 0;
    idt[n].flags = flags;
    idt[n].offset_2 = (address >> 16) & 0xFFFF;
    idt[n].offset_3 = (address >> 32) & 0xFFFFFFFF;
}

void load_idt(void) {
    idt_reg.size = IDT_ENTRIES * sizeof(idt_gate_descriptor_t) - 1;
    idt_reg.offset = (uint64_t)idt;
    __asm__ volatile ("lidt %0" : : "m" (idt_reg));
}