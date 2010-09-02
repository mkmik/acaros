#include <platform/hat/page_allocator.h>
#include <platform/paging.h>
#include <stand/libc.h>
#include <stand/stand.h>

static u_int32_t last_page;

void init_page_allocator_base() {
  last_page = (u_int32_t)stand_bootContext()->memory_map.allocatedSegments[0].base;
}

void init_page_allocator() {
  struct stand_physical_segment* high_segment = stand_getRamSegment(1);
  last_page = (u_int32_t)(high_segment->base + high_segment->length);
  last_page -= PAGE_SIZE*2; // don't know why
  
  stand_bootContext()->memory_map.allocatedCount = 1;
  stand_bootContext()->memory_map.allocatedSegments[0].length = 0;
  stand_bootContext()->memory_map.allocatedSegments[0].base = (void*)last_page; 
}

void* get_next_physical_page() {
  last_page -= PAGE_SIZE;

  stand_bootContext()->memory_map.allocatedSegments[0].length++;
  stand_bootContext()->memory_map.allocatedSegments[0].base = (void*)last_page; 

  return (void*)last_page;
}
