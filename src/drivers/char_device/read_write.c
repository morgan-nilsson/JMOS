#include "drivers/char_device/read_write.h"
#include "drivers/char_device/tui_device.h"

void (*init_char_device)(char_device_t* dev, ...) = NULL;
void (*close_char_device)(char_device_t* dev) = NULL;


void char_device_init(void) {
    init_char_device = init_tui_char_device;
    close_char_device = close_tui_char_device;
}