#include <stddef.h>
#include "drivers/font/font.h"
#include "drivers/font/font_data.h"
#include "drivers/font/psf2.h"
#include "memory/kalloc.h"

// Sole definition of the global declared extern in font.h.
font* global_font = NULL;

void init_font_format() {
    if (global_font == NULL) {
        global_font = kmalloc(sizeof(font));
        if (global_font == NULL) {
            return;
        }
    }
    init_psf2_font(global_font, (void*)spleen_font_start);
}