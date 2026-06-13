#ifndef _tui_device_h
#define _tui_device_h

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#include "drivers/char_device/read_write.h"

typedef struct {
    struct limine_framebuffer *framebuffer;
    unsigned int cursor_x;
    unsigned int cursor_y;
} char_device_tui_data_t;

void init_tui_char_device(char_device_t* dev, ...);
void close_tui_char_device(char_device_t* dev);

#endif