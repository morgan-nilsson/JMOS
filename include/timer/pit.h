#ifndef _pic_h
#define _pic_h

#include "libs/stdio.h"
#include "interrupt/isr.h"
#include "libs/registers.h"
#include "libs/errorno.h"

#define TIMER_CALLBACK_MAX_HANDLERS 16

typedef void (*tick_callback_t)(interrupt_registers_t* regs);

void init_timer(uint64_t freq);
uint64_t get_tick();

/// @brief  
/// @param callback NULL to unregister, valid function pointer to register a callback
/// @return index of the registered callback, or -1 if registration failed
int register_timer_callback(tick_callback_t callback);



#endif