#ifndef _PAGE_ALLOCATOR_H_
#define _PAGE_ALLOCATOR_H_

#include <types.h>

void init_page_allocator();
void init_page_allocator_base();
void* get_next_physical_page();

#endif
