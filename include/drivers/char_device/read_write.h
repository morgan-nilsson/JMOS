#ifndef _WRITE_READ_H
#define _WRITE_READ_H

#include <stdint.h>
#include <stddef.h>

typedef struct char_device {
    void (*write)(struct char_device* dev, const char* buf, size_t count);
    uint64_t (*read)(struct char_device* dev, char* buf, size_t count);
    void *data;
} char_device_t;

extern void (*init_char_device)(char_device_t* dev, ...);
extern void (*close_char_device)(char_device_t* dev);

void char_device_init(void);

#endif