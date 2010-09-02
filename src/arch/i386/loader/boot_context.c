#include "boot_context.h"
#include <platform/hat/page_allocator.h>
#include <platform/hat/initial_paging.h>
#include <hat/initial_paging.h>
#include <stand/libc.h>
#include <string.h>
#include "multiboot.h"
#include "tar.h"
#include "loader.h"
#include <log/log.h>

struct stand_boot_context boot_context;
struct stand_module modules[LOADER_MAX_MODULES];
struct stand_physical_segment segments[LOADER_MAX_SEGMENTS];
struct stand_physical_segment allocatedSegments[LOADER_MAX_SEGMENTS];
struct stand_virtual_segment vsegments[LOADER_MAX_SEGMENTS];

void init_boot_context() {
  boot_context.modules.count = 0;
  boot_context.modules.modules = modules;
  boot_context.memory_map.count = 0;
  boot_context.memory_map.segments = segments;
  boot_context.memory_map.allocatedCount = 0;
  boot_context.memory_map.allocatedSegments = allocatedSegments;
  boot_context.virtual_map.count = 0;
  boot_context.virtual_map.segments = vsegments;
  INIT_LIST_HEAD(&boot_context.virtual_map.ordered);
  boot_context.cmdline = multiboot_cmdline();
}

/** creates a copy of the boot_context in virtual memory, 
 * so that the kernel can easly unmap the intial paging 
 * as soon as it runs. 
 *  chicken in egg problem: virtual memory cannot be used until we have
 * a working boot_context. */
struct stand_boot_context* virtual_boot_context() {
  // compute the size (we don't have a memory allocator so we need to round 
  // to a page size and allocate a fixed number of pages)

  //  sum the size of all vsegment tags....
  int i;
  int strbuflen = 0, moduledatalen = 0;
  for(i=0; i<boot_context.modules.count; i++) {
    strbuflen += strlen(boot_context.modules.modules[i].name) + 1;
    moduledatalen += boot_context.modules.modules[i].size;
  }
  for(i=0; i<boot_context.virtual_map.count; i++) 
    strbuflen += strlen(boot_context.virtual_map.segments[i].tag) + 1;

  int size = sizeof(boot_context) + sizeof(modules) + sizeof(segments)
    + sizeof(allocatedSegments) + sizeof(vsegments) + strlen(boot_context.cmdline) + 1
    + strbuflen + moduledatalen;
  int numPages = (size-1) / PAGE_SIZE + 1;

  // allocate virtual memory

  struct stand_boot_context *copy = (void*)(HYPERSPACE - LOADER_INIT_STACK_SIZE - 
					    numPages * PAGE_SIZE);
  stand_reserveVirtualRange(copy, numPages, 0, "stand boot context");
  hat_initialMapAnonPages((pointer_t)copy, numPages, PROT_WRITE, "stand boot context");

  // begin the copy
  
  char* last = (char*)copy;
  memcpy(last, &boot_context, sizeof(boot_context));
  last += sizeof(boot_context);

  memcpy(last, modules, sizeof(modules));
  copy->modules.modules = (struct stand_module*)last;
  last += sizeof(modules);

  for(i=0; i<boot_context.modules.count; i++) {
    int len = strlen(boot_context.modules.modules[i].name) + 1;
    memcpy(last, boot_context.modules.modules[i].name,  len);	   
    copy->modules.modules[i].name = (char*)last;
    last += len;

    // TODO: modules are on page boundary! map instead of copying
    len =  boot_context.modules.modules[i].size;
    memcpy(last, boot_context.modules.modules[i].data,  len);
    copy->modules.modules[i].data = (void*)last;
    last += size;
  }

  memcpy(last, segments, sizeof(segments));
  copy->memory_map.segments = (struct stand_physical_segment*)last;
  last += sizeof(segments);

  memcpy(last, allocatedSegments, sizeof(allocatedSegments));
  copy->memory_map.allocatedSegments = (struct stand_physical_segment*)last;
  last += sizeof(allocatedSegments);

  memcpy(last, vsegments, sizeof(vsegments));
  copy->virtual_map.segments = (struct stand_virtual_segment*)last;
  last += sizeof(vsegments);

  INIT_LIST_HEAD(&copy->virtual_map.ordered);
  for(i=0; i<boot_context.virtual_map.count; i++) {
    int len = strlen(boot_context.virtual_map.segments[i].tag) + 1;
    memcpy(last, boot_context.virtual_map.segments[i].tag,  len);	   
    copy->virtual_map.segments[i].tag = (const char*)last;
    stand_virtualSegmentSortedInsert(copy, &copy->virtual_map.segments[i]);
    last += len;
  }
  
  memcpy(last, boot_context.cmdline, strlen(boot_context.cmdline) + 1);
  copy->cmdline = (const char*)last;


  return copy;
}

void fill_modules() {
  struct tar_archive* arch;

  arch = multiboot_get_module(0);
  if(arch==0)
    panic("cannot find kernel archive");
  if(!tar_is_valid_file(arch))
    panic("not valid tar archive");

  while(tar_is_valid_file(arch)) {
    modules[boot_context.modules.count].name = arch->block[0].header.name;
    modules[boot_context.modules.count].data = tar_file_data(arch);
    modules[boot_context.modules.count].size = tar_file_size(arch);

    boot_context.modules.count++;
    arch = tar_next_file(arch);
  }
}

void fill_memory_map() {
  struct multiboot_address_range* buffer= mb_info->mmap_addr;
  while(((char*)buffer - (char*)mb_info->mmap_addr) < mb_info->mmap_length) {
    segments[boot_context.memory_map.count].base = (void*)buffer->base_addr_low;
    segments[boot_context.memory_map.count].length = buffer->length_low;
    segments[boot_context.memory_map.count].type = buffer->type == 1 ? RAM : RESERVED;

    boot_context.memory_map.count++;
    buffer = (struct multiboot_address_range*)
      (((char*)buffer)+buffer->size+4);
  }
}

void fill_boot_context() {
  fill_modules();
  fill_memory_map();
}
