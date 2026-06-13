#include "memory/kalloc.h"
#include "memory/pmm.h"
#include "memory/bootinfo.h"
#include "libs/string.h"

// A first-fit free-list heap.

#define HEAP_ALIGN   16ull
#define MIN_GROW     (64 * 1024ull)   // grab at least this much per growth

typedef struct block {
    size_t        size;   // payload bytes (excludes this header)
    struct block *next;   // next free block (address-ordered); junk when in use
} block_t;

_Static_assert(sizeof(block_t) == HEAP_ALIGN, "block header must stay 16 bytes");

static block_t *free_head;

static inline size_t align_up(size_t v, size_t a) { return (v + a - 1) & ~(a - 1); }

static inline uint8_t *block_end(block_t *b) {
    return (uint8_t *)(b + 1) + b->size;
}

// Insert `b` into the address-ordered free list, coalescing with either
// physically-adjacent neighbour.
static void free_insert(block_t *b) {
    block_t *prev = NULL;
    block_t *cur  = free_head;
    while (cur != NULL && cur < b) {
        prev = cur;
        cur  = cur->next;
    }

    // Stitch b in between prev and cur.
    b->next = cur;
    if (prev != NULL) {
        prev->next = b;
    } else {
        free_head = b;
    }

    // Coalesce forward into cur.
    if (cur != NULL && block_end(b) == (uint8_t *)cur) {
        b->size += sizeof(block_t) + cur->size;
        b->next  = cur->next;
    }
    // Coalesce backward from prev.
    if (prev != NULL && block_end(prev) == (uint8_t *)b) {
        prev->size += sizeof(block_t) + b->size;
        prev->next  = b->next;
    }
}

// Pull a fresh contiguous run from the PMM and add it to the free list.
static int heap_grow(size_t need) {
    size_t bytes = need + sizeof(block_t);
    if (bytes < MIN_GROW) {
        bytes = MIN_GROW;
    }
    size_t frames = (bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    uint64_t phys = alloc_frames(frames);
    if (phys == 0) {
        return 0;
    }

    block_t *b = (block_t *)(phys + hhdm_off);
    b->size = frames * PAGE_SIZE - sizeof(block_t);
    free_insert(b);
    return 1;
}

void kheap_init(void) {
    free_head = NULL;
    heap_grow(MIN_GROW);
}

void *kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    size = align_up(size, HEAP_ALIGN);

    for (;;) {
        block_t *prev = NULL;
        for (block_t *b = free_head; b != NULL; prev = b, b = b->next) {
            if (b->size < size) {
                continue;
            }

            // Split when the remainder can hold a header plus a minimal payload.
            if (b->size >= size + sizeof(block_t) + HEAP_ALIGN) {
                block_t *rest = (block_t *)((uint8_t *)(b + 1) + size);
                rest->size = b->size - size - sizeof(block_t);
                rest->next = b->next;
                b->size    = size;
                if (prev != NULL) {
                    prev->next = rest;
                } else {
                    free_head = rest;
                }
            } else {
                if (prev != NULL) {
                    prev->next = b->next;
                } else {
                    free_head = b->next;
                }
            }
            return (void *)(b + 1);
        }

        // Nothing fit; grow and try again.
        if (!heap_grow(size)) {
            return NULL;
        }
    }
}

void *kcalloc(size_t n, size_t size) {
    if (n != 0 && size > (size_t)-1 / n) {
        return NULL; // multiplication would overflow
    }
    size_t total = n * size;
    void *p = kmalloc(total);
    if (p != NULL) {
        memset(p, 0, total);
    }
    return p;
}

void kfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    block_t *b = (block_t *)ptr - 1;
    free_insert(b);
}

void *krealloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return kmalloc(size);
    }
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }

    block_t *b = (block_t *)ptr - 1;
    size_t old = b->size;
    if (old >= align_up(size, HEAP_ALIGN)) {
        return ptr; // current block already big enough
    }

    void *new_ptr = kmalloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    memcpy(new_ptr, ptr, old < size ? old : size);
    kfree(ptr);
    return new_ptr;
}
