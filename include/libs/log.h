#ifndef _LOG_H
#define _LOG_H

#include <stdint.h>
#include "libs/serial.h"

#define LOG_NONE  0
#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4
#define LOG_TRACE 5

// Compile-time verbosity threshold. Override per build with -DLOG_LEVEL=N
// (see the makefile). Levels above this fold to nothing at compile time.
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO
#endif

// Emit `stmts` only when `level` is enabled. Because LOG_LEVEL is a constant,
// the compiler drops the whole block for disabled levels — zero runtime cost.
#define LOG_AT(level, stmts) \
    do { if ((level) <= LOG_LEVEL) { stmts } } while (0)

#define LOG_MSG(level, msg) \
    LOG_AT(level, serial_puts(msg); serial_putc('\n');)

#define LOG_HEX(level, label, val) \
    LOG_AT(level, serial_puts(label); serial_putc(' '); \
                  serial_put_hex((uint64_t)(val)); serial_putc('\n');)

#define LOG_DEC(level, label, val) \
    LOG_AT(level, serial_puts(label); serial_putc(' '); \
                  serial_put_dec((uint64_t)(val)); serial_putc('\n');)

#endif
