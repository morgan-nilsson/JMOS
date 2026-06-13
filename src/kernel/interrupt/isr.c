#include "interrupt/isr.h"
#include "interrupt/idt.h"
#include "libs/stdio.h"
#include "libs/serial.h"

isr_t interrupt_handlers[IDT_ENTRIES];

void isr_install() {
    // internal isrs
    set_idt_gate_descriptor(0, (uint64_t)isr0, 0x08, 0xE, 0);
    set_idt_gate_descriptor(1, (uint64_t)isr1, 0x08, 0xE, 0);
    set_idt_gate_descriptor(2, (uint64_t)isr2, 0x08, 0xE, 0);
    set_idt_gate_descriptor(3, (uint64_t)isr3, 0x08, 0xE, 0);
    set_idt_gate_descriptor(4, (uint64_t)isr4, 0x08, 0xE, 0);
    set_idt_gate_descriptor(5, (uint64_t)isr5, 0x08, 0xE, 0);
    set_idt_gate_descriptor(6, (uint64_t)isr6, 0x08, 0xE, 0);
    set_idt_gate_descriptor(7, (uint64_t)isr7, 0x08, 0xE, 0);
    set_idt_gate_descriptor(8, (uint64_t)isr8, 0x08, 0xE, 0);
    set_idt_gate_descriptor(9, (uint64_t)isr9, 0x08, 0xE, 0);
    set_idt_gate_descriptor(10, (uint64_t)isr10, 0x08, 0xE, 0);
    set_idt_gate_descriptor(11, (uint64_t)isr11, 0x08, 0xE, 0);
    set_idt_gate_descriptor(12, (uint64_t)isr12, 0x08, 0xE, 0);
    set_idt_gate_descriptor(13, (uint64_t)isr13, 0x08, 0xE, 0);
    set_idt_gate_descriptor(14, (uint64_t)isr14, 0x08, 0xE, 0);
    set_idt_gate_descriptor(15, (uint64_t)isr15, 0x08, 0xE, 0);
    set_idt_gate_descriptor(16, (uint64_t)isr16, 0x08, 0xE, 0);
    set_idt_gate_descriptor(17, (uint64_t)isr17, 0x08, 0xE, 0);
    set_idt_gate_descriptor(18, (uint64_t)isr18, 0x08, 0xE, 0);
    set_idt_gate_descriptor(19, (uint64_t)isr19, 0x08, 0xE, 0);
    set_idt_gate_descriptor(20, (uint64_t)isr20, 0x08, 0xE, 0);
    set_idt_gate_descriptor(21, (uint64_t)isr21, 0x08, 0xE, 0);
    set_idt_gate_descriptor(22, (uint64_t)isr22, 0x08, 0xE, 0);
    set_idt_gate_descriptor(23, (uint64_t)isr23, 0x08, 0xE, 0);
    set_idt_gate_descriptor(24, (uint64_t)isr24, 0x08, 0xE, 0);
    set_idt_gate_descriptor(25, (uint64_t)isr25, 0x08, 0xE, 0);
    set_idt_gate_descriptor(26, (uint64_t)isr26, 0x08, 0xE, 0);
    set_idt_gate_descriptor(27, (uint64_t)isr27, 0x08, 0xE, 0);
    set_idt_gate_descriptor(28, (uint64_t)isr28, 0x08, 0xE, 0);
    set_idt_gate_descriptor(29, (uint64_t)isr29, 0x08, 0xE, 0);
    set_idt_gate_descriptor(30, (uint64_t)isr30, 0x08, 0xE, 0);
    set_idt_gate_descriptor(31, (uint64_t)isr31, 0x08, 0xE, 0);

    // pic remapped

    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    // ICW3
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // OCW1 - unmask all interrupts
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // set pic irqs
    set_idt_gate_descriptor(IRQ0, (uint64_t)irq0, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 1, (uint64_t)irq1, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 2, (uint64_t)irq2, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 3, (uint64_t)irq3, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 4, (uint64_t)irq4, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 5, (uint64_t)irq5, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 6, (uint64_t)irq6, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 7, (uint64_t)irq7, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 8, (uint64_t)irq8, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 9, (uint64_t)irq9, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 10, (uint64_t)irq10, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 11, (uint64_t)irq11, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 12, (uint64_t)irq12, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 13, (uint64_t)irq13, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 14, (uint64_t)irq14, 0x08, 0xE, 0);
    set_idt_gate_descriptor(IRQ0 + 15, (uint64_t)irq15, 0x08, 0xE, 0);

    load_idt();
}

char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Bounds Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(interrupt_registers_t* regs) {
    uint64_t cr2;
    __asm__ volatile ("mov %0, cr2" : "=r" (cr2));

    serial_puts("\n*** CPU EXCEPTION ***\n");
    serial_puts("  vector  : ");
    serial_put_dec(regs->int_no);
    serial_puts(" (");
    serial_puts(regs->int_no < 32 ? exception_messages[regs->int_no] : "Unknown");
    serial_puts(")\n");
    serial_puts("  err_code: "); serial_put_hex(regs->err_code); serial_putc('\n');
    serial_puts("  rip     : "); serial_put_hex(regs->rip);       serial_putc('\n');
    serial_puts("  cs:rsp  : "); serial_put_hex(regs->cs);
    serial_putc(':');            serial_put_hex(regs->rsp);       serial_putc('\n');
    serial_puts("  rflags  : "); serial_put_hex(regs->rflags);    serial_putc('\n');
    serial_puts("  cr2     : "); serial_put_hex(cr2);             serial_putc('\n');
    serial_puts("  rax="); serial_put_hex(regs->rax);
    serial_puts(" rbx="); serial_put_hex(regs->rbx);
    serial_puts(" rcx="); serial_put_hex(regs->rcx); serial_putc('\n');
    serial_puts("  rdx="); serial_put_hex(regs->rdx);
    serial_puts(" rsi="); serial_put_hex(regs->rsi);
    serial_puts(" rdi="); serial_put_hex(regs->rdi); serial_putc('\n');
    serial_puts("  rbp="); serial_put_hex(regs->rbp);
    serial_puts(" r8 ="); serial_put_hex(regs->r8);
    serial_puts(" r9 ="); serial_put_hex(regs->r9); serial_putc('\n');
    serial_puts("*** halted ***\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void irq_handler(interrupt_registers_t* regs) {
        // Send EOI to PICs
        if (regs->int_no >= IRQ0 + 8) {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
    
        if (interrupt_handlers[regs->int_no] != 0) {
            isr_t handler = interrupt_handlers[regs->int_no];
            handler(regs);
        }
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}