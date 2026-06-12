#include <stdint.h>
#include <stddef.h>
#include <limine.h>

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

    struct limine_framebuffer *fb =
        (framebuffer_request.response != NULL
         && framebuffer_request.response->framebuffer_count > 0)
            ? framebuffer_request.response->framebuffers[0]
            : NULL;
    struct limine_memmap_response *memmap = memmap_request.response;
    struct limine_hhdm_response   *hhdm   = hhdm_request.response;
    (void)memmap;
    (void)hhdm;

    if (fb != NULL) {
        for (uint64_t y = 0; y < fb->height; y++) {
            volatile uint32_t *row =
                (volatile uint32_t *)((volatile uint8_t *)fb->address + y * fb->pitch);
            for (uint64_t x = 0; x < fb->width; x++) {
                uint8_t r = (uint8_t)(x * 255u / fb->width);
                uint8_t g = (uint8_t)(y * 255u / fb->height);
                row[x] = ((uint32_t)r << fb->red_mask_shift)
                       | ((uint32_t)g << fb->green_mask_shift)
                       | ((uint32_t)0x50 << fb->blue_mask_shift);
            }
        }
    }

    hcf();
}
