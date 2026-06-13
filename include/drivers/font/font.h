#ifndef _font_h
#define _font_h

#include <stdint.h>
#include <stdbool.h>

typedef struct font_format {
    void* data;
    void* glyph_buffer;
    bool (*has_unicode_table)(struct font_format*);
    unsigned int (*glyph_width)(struct font_format*);
    unsigned int (*glyph_height)(struct font_format*);
    unsigned int (*bytes_per_glyph)(struct font_format*);
    const void* (*get_bitmap_for_glyph)(struct font_format*, uint32_t unicode_codepoint);
} font;

extern font* global_font;
void init_font_format();

#endif