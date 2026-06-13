[extern isr_handler]
[extern irq_handler]
[extern syscall_table]

isr_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp  ; Pass pointer to the stack as argument to the handler

    call isr_handler
    add rsp, 16 ; discard int_no + error_code
    iretq

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; 0: Divide by zero
isr0:
    push byte 0
    push byte 0
    jmp isr_common_stub

; 1: Debug
isr1:
    push byte 1
    push byte 0
    jmp isr_common_stub

; 2: Non-maskable interrupt
isr2:
    push byte 2
    push byte 0
    jmp isr_common_stub

; 3: Breakpoint
isr3:
    push byte 3
    push byte 0
    jmp isr_common_stub


; 4: overflow
isr4:
    push byte 0
    push byte 4
    jmp isr_common_stub

; 5: bound range exceeded
isr5:
    push byte 0
    push byte 5
    jmp isr_common_stub

; 6: invalid opcode
isr6:
    push byte 0
    push byte 6
    jmp isr_common_stub

; 7: device not available
isr7:
    push byte 0
    push byte 7
    jmp isr_common_stub

; 8: double fault
isr8:
    push byte 0
    push byte 8
    jmp isr_common_stub

; 9: coprocessor segment overrun
isr9:
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: invalid TSS
isr10:
    push byte 0
    push byte 10
    jmp isr_common_stub

; 11: segment not present
isr11:
    push byte 0
    push byte 11
    jmp isr_common_stub

; 12: stack segment fault
isr12:
    push byte 0
    push byte 12
    jmp isr_common_stub

; 13: general protection fault
isr13:
    push byte 0
    push byte 13
    jmp isr_common_stub

; 14: page fault
isr14:
    push byte 0
    push byte 14
    jmp isr_common_stub

; 15: reserved
isr15:
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: x87 FPU error
isr16:
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: alignment check
isr17:
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: machine check
isr18:
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: SIMD floating point exception
isr19:
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: virtualization exception
isr20:
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: reserved
isr21:
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: reserved
isr22:
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: reserved
isr23:
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: reserved
isr24:
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: reserved
isr25:
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: reserved
isr26:
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: reserved
isr27:
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: reserved
isr28:
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: reserved
isr29:
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: reserved
isr30:
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: reserved
isr31:
    push byte 0
    push byte 31
    jmp isr_common_stub

irq_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp  ; Pass pointer to the stack as argument to the handler

    call irq_handler

    add rsp, 8 ; discard int_no

    ; Send End of Interrupt (EOI) signal to PICs here if needed

    iretq

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; 0: Timer
irq0:
    push byte 0
    push byte 32
    jmp irq_common_stub

; 1: Keyboard
irq1:
    push byte 1
    push byte 33
    jmp irq_common_stub

; 2: Cascade (used internally by the PICs)
irq2:
    push byte 2
    push byte 34
    jmp irq_common_stub

; 3: COM2
irq3:
    push byte 3
    push byte 35
    jmp irq_common_stub

; 4: COM1
irq4:
    push byte 4
    push byte 36
    jmp irq_common_stub

; 5: LPT2
irq5:
    push byte 5
    push byte 37
    jmp irq_common_stub

; 6: Floppy disk
irq6:
    push byte 6
    push byte 38
    jmp irq_common_stub

; 7: LPT1 / Unreliable "spurious" interrupt
irq7:
    push byte 7
    push byte 39
    jmp irq_common_stub

; 8: CMOS real-time clock
irq8:
    push byte 8
    push byte 40
    jmp irq_common_stub

; 9: Free for peripherals / legacy SCSI / NIC
irq9:
    push byte 9
    push byte 41
    jmp irq_common_stub

; 10: Free for peripherals / SCSI / NIC/ RAID
irq10:
    push byte 10
    push byte 42
    jmp irq_common_stub

; 11: Free for peripherals / SCSI / NIC / RAID
irq11:
    push byte 11
    push byte 43
    jmp irq_common_stub

; 12: PS2 Mouse
irq12:
    push byte 12
    push byte 44
    jmp irq_common_stub

; 13: FPU / Coprocessor / Inter-processor
irq13:
    push byte 13
    push byte 45
    jmp irq_common_stub

; 14: Primary ATA Hard Disk
irq14:
    push byte 14
    push byte 46
    jmp irq_common_stub

; 15: Secondary ATA Hard Disk
irq15:
    push byte 15
    push byte 47
    jmp irq_common_stub

global syscall_handler
syscall_handler:
    ; rax: syscall number
    ; rdi, rsi, rdx, r10, r8, r9: syscall arguments

    ; save all registers except rax (will contain result)
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; push the syscall parameters
    push r9
    push r8
    push r10
    push rdx
    push rsi
    push rdi

    mov rdi, rsp
    call [syscall_table + rax*8]

    ; clean up the stack
    add rsp, 48

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx

    iretq
