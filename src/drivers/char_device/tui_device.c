#include "drivers/char_device/tui_device.h"
#include <limine.h>
#include <stddef.h>
#include <stdarg.h>
#include "drivers/font/font.h"
#include "memory/kalloc.h"

void write_to_tui(char_device_t* dev, const char* buf, size_t count) {
    char_device_tui_data_t* data = (char_device_tui_data_t*)dev->data;
    if (data == NULL) {
        return;
    }

    struct limine_framebuffer* fb = data->framebuffer;
    if (fb == NULL) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const unsigned int cursor_x_offset = (data->cursor_x) * (global_font->glyph_width(global_font));
        const unsigned int cursor_y_offset = (data->cursor_y) * (global_font->glyph_height(global_font));
        const void* glyph_bitmap = global_font->get_bitmap_for_glyph(global_font, (uint32_t)buf[i]);

        for (unsigned int y = 0; y < global_font->glyph_height(global_font); y++) {
            for (unsigned int x = 0; x < global_font->glyph_width(global_font); x++) {
                const unsigned int byte_index = (y * global_font->glyph_width(global_font) + x) / 8;
                const unsigned int bit_index = 7 - (x % 8);
                const bool pixel_on = ((const uint8_t*)glyph_bitmap)[byte_index] & (1 << bit_index);

                if (pixel_on) {
                    volatile uint32_t* pixel = (volatile uint32_t*)(fb->address + (cursor_y_offset + y) * fb->pitch + (cursor_x_offset + x) * (fb->bpp / 8));
                    *pixel = 0xFFFFFFFF; // White color
                }
            }
        }

        data->cursor_x++;
        if (data->cursor_x >= fb->width / global_font->glyph_width(global_font)) {
            data->cursor_x = 0;
            data->cursor_y++;
        }
    }

}

uint64_t read_from_tui(char_device_t* dev, char* buf, size_t count) {
    write_to_tui(dev, "TUI read not implemented\n", 27);
}

/**
 * Initialize the TUI character device. This impl will take the struct limine_framebuffer*
 */
void init_tui_char_device(char_device_t* dev, ...) {
    va_list args;
    va_start(args, dev);

    struct limine_framebuffer* fb = va_arg(args, struct limine_framebuffer*);

    va_end(args);

    char_device_tui_data_t* data = kmalloc(sizeof(char_device_tui_data_t));
    if (data == NULL) {
        return;
    }
    data->framebuffer = fb;
    data->cursor_x = 0;
    data->cursor_y = 0;

    dev->write = write_to_tui;
    dev->read = read_from_tui;
    dev->data = data;
}

void close_tui_char_device(char_device_t* dev) {
    if (dev->data != NULL) {
        kfree(dev->data);
        dev->data = NULL;
    }
}