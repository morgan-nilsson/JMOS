#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>
#include <stddef.h>

void outb(uint16_t port, uint8_t value);
void inb(uint16_t port, uint8_t* value);

void outw(uint16_t port, uint16_t value);
void inw(uint16_t port, uint16_t* value);

#endif