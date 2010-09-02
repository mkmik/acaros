#include <mm/pfd.h>
#include <mm/addressspace.h>
#include <platform/paging.h>
#include <hat/initial_paging.h>
#include <platform/hat/initial_paging.h>
#include <hat/hat.h>
#include <stand/stand.h>
#include <log/log.h>
#include <string.h>
#include <autoconf.h>

page_frame_database_t pfd;

void mm_initPfd() {
  u_int32_t i,j;

  INIT_LIST_HEAD(&pfd.bad_list);
  INIT_LIST_HEAD(&pfd.free_list);
  INIT_LIST_HEAD(&pfd.used_list);

  // find highest address in RAM map
  j=0;
  struct stand_physical_segment* current_segment;
  struct stand_physical_segment* high_segment = stand_getRamSegment(j++);
  while((current_segment = stand_getRamSegment(j++))) {
    if((pointer_t)(current_segment->base + current_segment->length) >
       (pointer_t)(high_segment->base + high_segment->length))
      high_segment = current_segment;
  }
  pfd.numFrames = (pointer_t)(high_segment->base + high_segment->length) / PAGE_SIZE;

  // alloc kernel heap for PFD
  pfd.frames = (page_frame_t*)stand_getNextFreeVirtual(stand_bootContext()->kernelBase,
						       pfd.numFrames*sizeof(page_frame_t) /
						       PAGE_SIZE+1);
  hat_initialMapAnonPages((pointer_t)pfd.frames,
			  pfd.numFrames*sizeof(page_frame_t)/PAGE_SIZE+1,
			  PROT_WRITE, "PFD");

  memset(pfd.frames, 0, pfd.numFrames*sizeof(page_frame_t));

  // put all memory in bad list
  for(i=0; i<pfd.numFrames; i++) {
    list_add(&pfd.frames[i].link, &pfd.bad_list);
  }

  // put ram segments in free list
  j=0;
  struct stand_physical_segment* phs;
  while((phs = stand_getRamSegment(j++))) {
    pointer_t bottom = (pointer_t)phs->base;
    pointer_t top = bottom + (pointer_t)phs->length;
    bottom /= PAGE_SIZE;
    top /= PAGE_SIZE;
    for(i=bottom; i<top; i++) 
      mm_pfd_setFrameState(i, free);
  }

  // don't know why but top 2 (+2 magic) pages are unusable
  for(i=pfd.numFrames-4; i<pfd.numFrames; i++)
    mm_pfd_setFrameState(i, bad);

  // init used list

  struct stand_boot_context* ctx = stand_bootContext();
  for(i=0; i<ctx->virtual_map.count; i++) {
    u_int8_t* base = ctx->virtual_map.segments[i].base;
    klogf(LOG_MESSAGE, "PFD: adding %p:0x%x '%s' to used list\n", 
	  base, ctx->virtual_map.segments[i].length * PAGE_SIZE, 
	  ctx->virtual_map.segments[i].tag);
    
    for(j=0; j<ctx->virtual_map.segments[i].length; j++, base += PAGE_SIZE) {
      if(hat_isPresent(base))
	mm_pfd_setFrameState(mm_pageFrameNumber(hat_virtualToPhysical(base)), used);
    }
  }

#ifdef CONFIG_PFD_STAND_CHECK
  // DEBUG!
  u_int8_t *base;

  base = (void*)0x400000;
  klogf(LOG_MESSAGE, "SCANNING ALL VIRTUAL SPACE (4GB!) (%p->%p)\n",
	base, base + PAGE_SIZE*((1LU << 20) - 1024 - 1));

  for(j=0; j<((1<<20) - 1024); j++, base += PAGE_SIZE) {
    if(hat_isPresent(base)) {
      list_head_t *pos;
      list_for_each(pos, &pfd.free_list) {
	page_frame_t *page = list_entry(pos, page_frame_t, link);
	if((page - pfd.frames) == mm_pageFrameNumber(hat_virtualToPhysical(base))) {
	  klogf(LOG_MESSAGE, "virtual space not free! %p physical 0x%x\n", base, hat_virtualToPhysical(base));
	  panic("PAGE PRESENT BUT STILL IN FREE LIST");
	  mm_pfd_setFrameState(mm_pageFrameNumber(hat_virtualToPhysical(base)), used);
	  break;
	}
      }

    }
  }
#endif

  // debug

  page_frame_t* firstFree = list_entry(pfd.free_list.next, page_frame_t, link);
  klogf(LOG_MESSAGE, "first free frame 0x%x addr 0x%x\n",
       firstFree - pfd.frames,
       (firstFree - pfd.frames) * PAGE_SIZE);

  // switch hat to use pfd
  hat_initial_get_next_physical_page = mm_allocPhysicalPage;
}

void* mm_allocPhysicalPage() {
  // statistics
  static u_int32_t pagesAllocated = 0;

  klogf(LOG_DEBUG, "Allocating physical page ");

  if(list_empty(&pfd.free_list)) {
    panic("EMPTY FREE LIST");
    return (void*)-1;
  }
    
  page_frame_t* firstFree = list_entry(pfd.free_list.next, page_frame_t, link);
  mm_pfd_setFrameState((firstFree - pfd.frames), used);

  pagesAllocated++;
  klogf(LOG_DEBUG, "%p (allocated %d)\n", (void*)((firstFree - pfd.frames) * PAGE_SIZE),
	pagesAllocated);
  return (void*)((firstFree - pfd.frames) * PAGE_SIZE);
}

void* mm_allocPhysicalPageBottom() {
  // statistics
  static u_int32_t pagesAllocated = 0;

  klogf(LOG_DEBUG, "Allocating physical page bottom");

  if(list_empty(&pfd.free_list)) {
    panic("EMPTY FREE LIST");
    return (void*)-1;
  }
    
  page_frame_t* firstFree = list_entry(pfd.free_list.prev, page_frame_t, link);
  mm_pfd_setFrameState((firstFree - pfd.frames), used);

  pagesAllocated++;
  klogf(LOG_DEBUG, "%p (allocated bottom%d)\n", (void*)((firstFree - pfd.frames) * PAGE_SIZE),
	pagesAllocated);
  return (void*)((firstFree - pfd.frames) * PAGE_SIZE);
}
