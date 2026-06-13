#include <stddef.h>

#include "drivers/font/psf2.h"
#include "memory/kalloc.h"
#include "libs/string.h"

bool psf2_has_unicode_table(font* font) {
    psf2_header* psf2_font = (psf2_header*)font->data;
    return (psf2_font->flags & PSF2_HAS_UNICODE_TABLE) != 0;
}

unsigned int psf2_glyph_width(font* font) {
    psf2_header* psf2_font = (psf2_header*)font->data;
    return psf2_font->width;
}

unsigned int psf2_glyph_height(font* font) {
    psf2_header* psf2_font = (psf2_header*)font->data;
    return psf2_font->height;
}

unsigned int psf2_bytes_per_glyph(font* font) {
    psf2_header* psf2_font = (psf2_header*)font->data;
    return psf2_font->bytesperglyph;
}

const void* psf2_get_bitmap_for_glyph(font* font, uint32_t unicode_codepoint) {
    psf2_header* psf2_font = (psf2_header*)font->data;
    if (unicode_codepoint >= psf2_font->numglyph) {
        return NULL; // Glyph not found
    }
    return (void*)((uintptr_t)font->glyph_buffer + unicode_codepoint * psf2_font->bytesperglyph);
}

/// @brief Initialize a font struct with the given raw data
/// @param font 
/// @param font_data pointer to the raw PSF2 font file buffer
errno_t init_psf2_font(font* font, void* font_data) {
    psf2_header* psf2 = kmalloc(sizeof(psf2_header));
    if (psf2 == NULL) {
        return ENOMEM; // Allocation failed
    }

    // Copy the header data from the raw font data
    psf2_header* raw_psf2 = (psf2_header*)font_data;
    psf2->magic = raw_psf2->magic;
    if (psf2->magic != PSF2_MAGIC) {
        kfree(psf2);
        return EINVAL; // Invalid font format
    }
    psf2->version = raw_psf2->version;
    psf2->headersize = raw_psf2->headersize;
    psf2->flags = raw_psf2->flags;
    psf2->numglyph = raw_psf2->numglyph;
    psf2->bytesperglyph = raw_psf2->bytesperglyph;
    psf2->height = raw_psf2->height;
    psf2->width = raw_psf2->width;

    // The glyph buffer starts immediately after the header
    void* glyph_buffer = (void*)((uintptr_t)font_data + psf2->headersize);
    // copy the glyph buffer into kernel memory
    size_t glyph_buffer_size = psf2->numglyph * psf2->bytesperglyph;
    font->glyph_buffer = kmalloc(glyph_buffer_size);
    if (font->glyph_buffer == NULL) {
        kfree(psf2);
        return ENOMEM; // Allocation failed
    }
    memcpy(font->glyph_buffer, glyph_buffer, glyph_buffer_size);

    font->data = psf2;
    font->has_unicode_table = psf2_has_unicode_table;
    font->glyph_width = psf2_glyph_width;
    font->glyph_height = psf2_glyph_height;
    font->bytes_per_glyph = psf2_bytes_per_glyph;
    font->get_bitmap_for_glyph = psf2_get_bitmap_for_glyph;

    return 0; // Success
}

errno_t close_psf2_font(font* font) {
    if (font == NULL || font->data == NULL) {
        return EINVAL; // Invalid font
    }
    kfree(font->data);
    font->data = NULL;
}