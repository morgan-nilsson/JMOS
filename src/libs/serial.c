#include "libs/serial.h"
#include "libs/stdio.h"

#define COM1 0x3F8

static int serial_ready = 0;

void serial_init(void) {
    outb(COM1 + 1, 0x00);   // disable interrupts
    outb(COM1 + 3, 0x80);   // enable DLAB (set baud divisor)
    outb(COM1 + 0, 0x03);   // divisor low  -> 38400 baud
    outb(COM1 + 1, 0x00);   // divisor high
    outb(COM1 + 3, 0x03);   // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7);   // enable FIFO, clear, 14-byte threshold
    outb(COM1 + 4, 0x0B);   // IRQs enabled, RTS/DSR set
    serial_ready = 1;
}

static int tx_empty(void) {
    uint8_t status;
    inb(COM1 + 5, &status);
    return status & 0x20;   // line status: transmitter holding register empty
}

void serial_putc(char c) {
    if (!serial_ready) {
        return;
    }
    if (c == '\n') {
        while (!tx_empty()) { }
        outb(COM1, '\r');
    }
    while (!tx_empty()) { }
    outb(COM1, (uint8_t)c);
}

void serial_puts(const char *s) {
    if (s == NULL) {
        return;
    }
    while (*s) {
        serial_putc(*s++);
    }
}

void serial_put_hex(uint64_t v) {
    static const char digits[] = "0123456789ABCDEF";
    serial_putc('0');
    serial_putc('x');
    for (int shift = 60; shift >= 0; shift -= 4) {
        serial_putc(digits[(v >> shift) & 0xF]);
    }
}

void serial_put_dec(uint64_t v) {
    char buf[20];
    int i = 0;
    if (v == 0) {
        serial_putc('0');
        return;
    }
    while (v > 0) {
        buf[i++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (i > 0) {
        serial_putc(buf[--i]);
    }
}
