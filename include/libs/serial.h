#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>

// COM1 16550 UART logging over the QEMU `-serial stdio` channel.
void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);
void serial_put_hex(uint64_t v);   // prints 0x-prefixed, zero-padded to width
void serial_put_dec(uint64_t v);

#endif
