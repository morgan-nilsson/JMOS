#ifndef _syscall_h
#define _syscall_h

#include <stdint.h>

void init_syscalls();
int register_syscall(void (*handler)());
int syscall(uint8_t syscall_number, ...);


#endif