#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

void outb(uint16_t port, uint8_t value);
void inb(uint16_t port, uint8_t* value);

void outw(uint16_t port, uint16_t value);
void inw(uint16_t port, uint16_t* value);

int printf(const char* restrict format, ...);
int sprintf( char* restrict buffer, const char* restrict format, ... );
int vsprintf( char* restrict buffer, const char* restrict format, va_list args );
int snprintf( char* restrict buffer, size_t bufsz,
    const char* restrict format, ... );

void perror(const char* s);


#endif