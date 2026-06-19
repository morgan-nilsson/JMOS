#include "libs/stdio.h"
#include "drivers/char_device/read_write.h"
#include "libs/stdlib.h"
#include "libs/string.h"

#include <stdarg.h>

void outb(uint16_t port, uint8_t value) {
#if defined(__x86_64__)
    __asm__ volatile ("out %1, %0" : : "a" (value), "Nd" (port));
#else
    (void)port; (void)value;
#endif
}

void inb(uint16_t port, uint8_t* value) {
#if defined(__x86_64__)
    __asm__ volatile ("in %0, %1" : "=a" (*value) : "Nd" (port));
#else
    (void)port; *value = 0;
#endif
}

void outw(uint16_t port, uint16_t value) {
#if defined(__x86_64__)
    __asm__ volatile ("out %1, %0" : : "a" (value), "Nd" (port));
#else
    (void)port; (void)value;
#endif
}

void inw(uint16_t port, uint16_t* value) {
#if defined(__x86_64__)
    __asm__ volatile ("in %0, %1" : "=a" (*value) : "Nd" (port));
#else
    (void)port; *value = 0;
#endif
}

static unsigned int emit_uint(char *restrict *out, unsigned long long value, unsigned int base) {
    char tmp[65];   // worst case: 64 binary digits
    int i = 0;
    do {
        unsigned int d = (unsigned int)(value % base);
        tmp[i++] = (d < 10) ? (char)('0' + d) : (char)('a' + d - 10);
        value /= base;
    } while (value != 0);

    unsigned int n = (unsigned int)i;
    while (i > 0) {
        *(*out)++ = tmp[--i];   // digits were produced least-significant first
    }
    return n;
}

static unsigned int emit_int(char *restrict *out, long long value, unsigned int base) {
    if (value < 0) {
        *(*out)++ = '-';
        return 1 + emit_uint(out, 0ULL - (unsigned long long)value, base);
    }
    return emit_uint(out, (unsigned long long)value, base);
}

static unsigned int emit_str(char *restrict *out, const char* s) {
    unsigned int n = 0;
    while (*s) {
        *(*out)++ = *s++;
        n++;
    }
    return n;
}

int vsprintf( char* restrict buffer, const char* restrict format, va_list args ) {
    unsigned int count = 0;

    while (*format) {
        if (*format != '%') {
            *buffer++ = *format++;
            count++;
            continue;
        }

        ++format;   // skip '%'
        switch (*format) {
            case 'd':
                count += emit_int(&buffer, (long long)va_arg(args, int), 10);
                break;
            case 'u':
                count += emit_uint(&buffer, (unsigned long long)va_arg(args, unsigned int), 10);
                break;
            case 'x':
                count += emit_uint(&buffer, (unsigned long long)va_arg(args, unsigned int), 16);
                break;
            case 'c':
                *buffer++ = (char)va_arg(args, int);   // char promoted to int
                count++;
                break;
            case 's':
                count += emit_str(&buffer, va_arg(args, const char*));
                break;
            case '%':
                *buffer++ = '%';
                count++;
                break;
            case 'p':
                *buffer++ = '0';
                *buffer++ = 'x';
                count += 2;
                count += emit_uint(&buffer, (unsigned long long)(uintptr_t)va_arg(args, void*), 16);
                break;
            case 'l':   // long
                ++format;
                if (*format == 'd') {
                    count += emit_int(&buffer, (long long)va_arg(args, long int), 10);
                } else if (*format == 'u') {
                    count += emit_uint(&buffer, (unsigned long long)va_arg(args, unsigned long int), 10);
                } else if (*format == 'x') {
                    count += emit_uint(&buffer, (unsigned long long)va_arg(args, unsigned long int), 16);
                } else {
                    count += emit_str(&buffer, "(bad %l)");
                }
                break;
            case 'L':   // long long
                ++format;
                if (*format == 'd') {
                    count += emit_int(&buffer, va_arg(args, long long int), 10);
                } else if (*format == 'u') {
                    count += emit_uint(&buffer, va_arg(args, unsigned long long int), 10);
                } else if (*format == 'x') {
                    count += emit_uint(&buffer, va_arg(args, unsigned long long int), 16);
                } else {
                    count += emit_str(&buffer, "(bad %L)");
                }
                break;
            default:
                // Unknown specifier: echo the char (but not a trailing NUL).
                if (*format) {
                    *buffer++ = *format;
                    count++;
                }
                break;
        }

        if (*format) {
            ++format;
        }
    }

    *buffer = '\0'; // Null-terminate the buffer
    return count;
}

int sprintf( char* restrict buffer, const char* restrict format, ... ) {
    va_list args;
    va_start(args, format);
    const int count = vsprintf(buffer, format, args);
    va_end(args);
    return count;
}

/// @brief 
/// @param format 
/// @param  
/// @return 
int printf(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);

    // TODO: stream straight to the console to avoid this stack buffer
    char buffer[2048];
    const int count = vsprintf(buffer, format, args);
    va_end(args);

    console.write(&console, buffer, count);
    return count;
}

