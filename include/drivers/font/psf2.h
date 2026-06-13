#ifndef _psf2_h
#define _psf2_h

#include <stdint.h>
#include "drivers/font/font.h"
#include "libs/errorno.h"

#define PSF2_MAGIC 0x864ab572        // bytes on disk: 72 b5 4a 86
#define PSF2_HAS_UNICODE_TABLE 0x01

typedef struct {
    uint32_t magic;          // 0x864ab572
    uint32_t version;        // 0
    uint32_t headersize;     // byte offset where glyphs begin (usually 32)
    uint32_t flags;          // bit 0 = a unicode table follows the glyphs
    uint32_t numglyph;       // how many glyphs
    uint32_t bytesperglyph;  // size of one glyph in bytes
    uint32_t height;         // glyph height in pixels
    uint32_t width;          // glyph width in pixels
} psf2_header;

_Static_assert(sizeof(psf2_header) == 32, "PSF2 header must be 32 bytes");

errno_t init_psf2_font(font* font, void* font_data);
errno_t close_psf2_font(font* font);

#endif