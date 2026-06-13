#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

typedef struct {
    uint16_t offset_1;
    uint16_t segment_selector;
    unsigned int ist: 3;
    unsigned int _reserved: 5;
    uint8_t flags;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t _reserved2;
} idt_gate_descriptor_t;

typedef struct {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) idt_register_t;

void set_idt_gate_descriptor(uint8_t n, uint64_t address, uint16_t ss, uint8_t ist, uint16_t flags);
void load_idt(void);

#endif