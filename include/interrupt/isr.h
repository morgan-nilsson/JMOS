#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>
#include "libs/registers.h"
#include "interrupt/interrupt.h"

#define IDT_ENTRIES 256

typedef void (*isr_t)(interrupt_registers_t*);

void register_interrupt_handler(uint8_t n, isr_t handler);
void isr_install();
uint8_t get_pic_mask();

#define IRQ0 32


#endif