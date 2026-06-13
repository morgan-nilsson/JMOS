#include "libs/stdio.h"

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("out %1, %0" : : "a" (value), "Nd" (port));
}

void inb(uint16_t port, uint8_t* value) {
    __asm__ volatile ("in %0, %1" : "=a" (*value) : "Nd" (port));
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("out %1, %0" : : "a" (value), "Nd" (port));
}

void inw(uint16_t port, uint16_t* value) {
    __asm__ volatile ("in %0, %1" : "=a" (*value) : "Nd" (port));
}