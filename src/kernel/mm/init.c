#include <mm/mm.h>
#include <platform/paging.h>
#include <platform/hat/page_allocator.h>
#include <mm/pfd.h>
#include <mm/addressspace.h>
#include <mm/dma.h>
#include <log/log.h>
#include <stand/stand.h>
#include <hat/initial_paging.h>
#include <platform/mal/idt.h>
#include <mm/slab/slab.h>

static void tempPageFault(mal_vector_t vector, 
		      u_int32_t error,
		      u_int32_t eip) {
  void* addr;
  asm("mov %%cr2, %0" : "=r"(addr));

  klogf(LOG_MESSAGE, "(OLD HANDLER) Page fault at %p, error 0x%x, ip 0x%x\n", addr, error, eip);
  klogf(LOG_MESSAGE, "reasion: ");
  if(error & 0x1)
    klogf(LOG_MESSAGE, "present, ");
  else
    klogf(LOG_MESSAGE, "not present, ");
  if(error & 0x2)
    klogf(LOG_MESSAGE, "write, ");
  else
    klogf(LOG_MESSAGE, "read, ");
  if(error & 0x4)
    klogf(LOG_MESSAGE, "user\n");
  else
    klogf(LOG_MESSAGE, "supervisor\n");

  panic("page fault");
}

void mm_printInfo() {
  struct stand_boot_context* ctx = stand_bootContext();
  int i;
  for(i=0; i<ctx->memory_map.count; i++) {
    klogf(LOG_MESSAGE, "memory at %p - %p bytes %d flags %s\n",
	 ctx->memory_map.segments[i].base,
	 ctx->memory_map.segments[i].base +
	 ctx->memory_map.segments[i].length,
	 ctx->memory_map.segments[i].length,
	 ctx->memory_map.segments[i].type == RAM ? "ram" : "rom");
  }
  for(i=0; i<ctx->memory_map.allocatedCount; i++) {
    klogf(LOG_MESSAGE, "allocated at %p - %p pages %d\n",
	 ctx->memory_map.allocatedSegments[i].base,
	 ctx->memory_map.allocatedSegments[i].base +
	 ctx->memory_map.allocatedSegments[i].length * PAGE_SIZE,
	 ctx->memory_map.allocatedSegments[i].length);
  }
  for(i=0; i<ctx->virtual_map.count; i++) {
    klogf(LOG_MESSAGE, "vm at %p - %p pages %d flags %d tag '%s'\n",
	 ctx->virtual_map.segments[i].base,
	 ctx->virtual_map.segments[i].base +
	 ctx->virtual_map.segments[i].length * PAGE_SIZE,
	 ctx->virtual_map.segments[i].length,
	 ctx->virtual_map.segments[i].flags,
	 ctx->virtual_map.segments[i].tag);
  }
}

void mm_init() {

  init_page_allocator_base();

  mal_attachException(14, tempPageFault);

  mm_initPfd();
  mm_printInfo();

  mm_addressspace_init();
  mm_dma_init();

  mm_initFramebuffer();
  //  hat_unmapIdentityPages();

  slab_init();
}
