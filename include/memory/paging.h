#ifndef _paging_h
#define _paging_h

#include <stdint.h>

typedef uint64_t page_directory_entry_t;
typedef uint64_t page_table_entry_t;
typedef uint64_t virtual_address_t;
typedef uint64_t physical_address_t;

void map_page(uint64_t *pml4, virtual_address_t va, physical_address_t pa, uint64_t flags);

uint64_t vmm_clone_kernel_space(void);

void vmm_load_space(physical_address_t pml4_phys);

#endif