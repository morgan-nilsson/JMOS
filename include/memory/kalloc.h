#ifndef _kalloc_h
#define _kalloc_h

#include <stdint.h>
#include <stddef.h>

// Initialize the kernel heap
void kheap_init(void);

// Allocate `size` bytes (16-byte aligned). Returns NULL on failure.
void *kmalloc(size_t size);

// Allocate and zero `n * size` bytes. Returns NULL on failure or overflow.
void *kcalloc(size_t n, size_t size);

// Resize a previous allocation; semantics mirror C realloc.
void *krealloc(void *ptr, size_t size);

// Release an allocation
void kfree(void *ptr);

#endif
