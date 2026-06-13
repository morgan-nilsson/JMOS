#ifndef _stdlib_h
#define _stdlib_h

#include <stdint.h>
#include <stddef.h>
#include <stdnoreturn.h>
#include "libs/errorno.h"

#define __STDC_LIB_EXT1__ 0

int atoi(const char* nstr);
long int atol(const char* nptr);
long long int atoll(const char* nptr);
long int strtol(const char* restrict nptr, char** restrict endptr, int base);
long long int strtoll(const char* restrict nptr, char** restrict endptr, int base);
unsigned long int strtoul(const char* restrict nptr, char** restrict endptr, int base);
unsigned long long int strtoull(const char* restrict nptr,
                                char** restrict endptr, int base);

[[noreturn]] void abort(void);

int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);


#if __STDC_LIB_EXT1__
#if defined(__STDC_WANT_LIB_EXT1__)

#define DEFAULT_CONSTRAINT_HANDLER abort_handler_s

typedef void (*constraint_handler_t)( const char* restrict msg,
    void* restrict ptr,
    errno_t error );

constraint_handler_t contraint_handler = DEFAULT_CONSTRAINT_HANDLER;

constraint_handler_t set_constraint_handler_s(constraint_handler_t handler);
void abort_handler_s(const char* restrict msg, void* restrict ptr, errno_t error);
void ignore_handler_s(const char* restrict msg, void* restrict ptr, errno_t error);
#endif

#endif
#endif