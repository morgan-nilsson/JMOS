#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "memory/memory.h"
#include "memory/bootinfo.h"
#include "memory/kalloc.h"
#include "drivers/char_device/read_write.h"
#include "drivers/font/font.h"
#include "interrupt/isr.h"
#include "libs/serial.h"
#include "libs/log.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[3] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[4] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[2] = LIMINE_REQUESTS_END_MARKER;

static void hcf(void) {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void kernel_main(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        hcf();
    }

    serial_init();
    LOG_MSG(LOG_INFO, "[boot] kernel_main reached");

    isr_install();
    LOG_MSG(LOG_INFO, "[boot] IDT installed");

    struct limine_framebuffer *fb =
        (framebuffer_request.response != NULL
         && framebuffer_request.response->framebuffer_count > 0)
            ? framebuffer_request.response->framebuffers[0]
            : NULL;

    memory_init(memmap_request.response, hhdm_request.response, fb);
    LOG_MSG(LOG_INFO, "[boot] memory_init done");

    init_font_format();

    char_device_init();

    char_device_t console;
    init_char_device(&console, fb);

    console.write(&console, "Hello, JMOS!\n", 12);


    //const framebuffer_t *screen = bootinfo_framebuffer();
    //if (screen->present) {
    //    for (uint64_t y = 0; y < screen->height; y++) {
    //        volatile uint32_t *row =
    //            (volatile uint32_t *)(screen->address + y * screen->pitch);
    //        for (uint64_t x = 0; x < screen->width; x++) {
    //            uint8_t r = (uint8_t)(x * 255u / screen->width);
    //            uint8_t g = (uint8_t)(y * 255u / screen->height);
    //            row[x] = ((uint32_t)r << screen->red_shift)
    //                   | ((uint32_t)g << screen->green_shift)
    //                   | ((uint32_t)0x50 << screen->blue_shift);
    //        }
    //    }
    //}

    hcf();
}
