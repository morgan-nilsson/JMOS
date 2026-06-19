#ifndef _string_h
#define _string_h

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include "libs/errorno.h"


#ifndef RSIZE_MAX
#define RSIZE_MAX ((size_t)(-1) >> 1)
#endif

char* strcpy(char* restrict dest, const char* restrict src);
char* strncpy(char* restrict dest, const char* restrict src, size_t n);
char* strcat(char* restrict dest, const char* restrict src);
char* strncat(char* restrict dest, const char* restrict src, size_t n);
// size_t strxfrm(char* restrict dest, const char* restrict src, size_t n);
// char* strdup(const char* s);
// char* strndup(const char* s, size_t n);

size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
// int strcoll(const char* lhs, const char* rhs);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
size_t strcspn(const char* s, const char* reject);
size_t strspn(const char* s, const char* accept);
char* strpbrk(const char* s, const char* accept);
char* strstr(const char* haystack, const char* needle);
char* strtok(char* restrict str, const char* restrict delim);

void* memchr(const void* ptr, int ch, size_t n);
int memcmp(const void* ptr1, const void* ptr2, size_t n);
void* memset(void* ptr, int value, size_t n);
void* memcpy(void* restrict dest, const void* restrict src, size_t n);
void* memmove(void* dest, const void* src, size_t n);

char* strerror(errno_t errnum);

#endif