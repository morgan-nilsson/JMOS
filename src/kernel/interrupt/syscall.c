#include <stdarg.h>
#include "interrupt/syscall.h"
#include "interrupt/idt.h"
#include "interrupt/interrupt.h"

void (*syscall_table[256])();

void init_syscalls() {
    // Set the syscall handler in the IDT
    set_idt_gate_descriptor(0x80, (uint64_t)syscall_handler, 0x08, 0, 0xE);
}

int syscall(uint8_t syscall_number, ...) {
    // Syscall arguments are passed in rdi, rsi, rdx, r10, r8, r9
    // The syscall number is passed in rax

    va_list args;
    va_start(args, syscall_number);

    uint64_t arg1 = va_arg(args, uint64_t);
    uint64_t arg2 = va_arg(args, uint64_t);
    uint64_t arg3 = va_arg(args, uint64_t);
    uint64_t arg4 = va_arg(args, uint64_t);
    uint64_t arg5 = va_arg(args, uint64_t);
    uint64_t arg6 = va_arg(args, uint64_t);

    va_end(args);

    uint64_t result;
    register uint64_t r10 __asm__("r10") = arg4;
    register uint64_t r8  __asm__("r8")  = arg5;
    register uint64_t r9  __asm__("r9")  = arg6;
    __asm__ volatile (
        "int 0x80\n"
        : "=a" (result)
        : "a" ((uint64_t)syscall_number),
          "D" (arg1), "S" (arg2), "d" (arg3),
          "r" (r10), "r" (r8), "r" (r9)
        : "memory"
    );

    return (int)result;
}