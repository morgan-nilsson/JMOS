#include "timer/pit.h"

volatile __uint128_t _tick = 0;

static tick_callback_t _tick_callbacks[TIMER_CALLBACK_MAX_HANDLERS];
static size_t _tick_callback_count = 0;

static void timer_callback(interrupt_registers_t* regs) {
    ++_tick;

    for (size_t i = 0; i < _tick_callback_count; i++) {
        if (_tick_callbacks[i] != NULL) {
            _tick_callbacks[i](regs);
        }
    }

    outb(0x20, 0x20); // Send End of Interrupt (EOI) signal to PIC
}

void init_timer(uint64_t freq) {
    register_interrupt_handler(IRQ0, timer_callback);

    uint32_t divisor = 1193180 / freq;

    outb(0x43, 0x36);

    uint8_t low = divisor & 0xFF;
    uint8_t high = (divisor >> 8) & 0xFF;

    outb(0x40, low);
    outb(0x40, high);
}

uint64_t get_tick() {
    return _tick;
}

int register_timer_callback(tick_callback_t callback) {
    if (_tick_callback_count >= TIMER_CALLBACK_MAX_HANDLERS) {
        return -1; // No more space for callbacks
    }
    _tick_callbacks[_tick_callback_count++] = callback;
    return _tick_callback_count - 1; // Return the index of the registered callback
}
