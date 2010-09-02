#include <stand/stand.h>
#include <platform/paging.h>
#include <stand/libc.h>
#include <version.h>
#include <log/log.h>

static struct stand_boot_context* boot_context = 0;

void stand_printIntro() {
  cls();
  printf("Acaros " KERNEL_RELEASE "\n");
  printf("================\n");
}

void stand_init(struct stand_boot_context* ctx) {
  boot_context = ctx;
}

void* stand_findModule(char* name) {
  if(!boot_context)
    return 0;

  int i;
  for(i=0; i<boot_context->modules.count; i++) {
    if(strcmp(boot_context->modules.modules[i].name, name) == 0)
      return boot_context->modules.modules[i].data;
  }
  return 0;
}

struct stand_physical_segment* stand_getRamSegment(int idx) {
  int i;
  int r=0;
  for(i=0; i<boot_context->memory_map.count; i++) {
    if(boot_context->memory_map.segments[i].type == RAM) {
      if(r == idx)
	return &boot_context->memory_map.segments[i];
      r++;
    }
  }
  return 0;
}

struct stand_boot_context* stand_bootContext() {
  return boot_context;
}

void stand_virtualSegmentSortedInsert(struct stand_boot_context *context,
				      struct stand_virtual_segment *vs) {
  list_head_t *pos;
  list_for_each(pos, &context->virtual_map.ordered) {
    struct stand_virtual_segment *each = list_entry(pos, struct stand_virtual_segment, list);
    if(vs->base <= each->base)
      break;
  }
  list_add_tail(&vs->list, pos);
  //  klogf(LOG_DEBUG, "ADDING SEGMENT %p '%s' after item %p (head %p)\n", vs->base, vs->tag, pos, 
  //  	 &context->virtual_map.ordered);
  
}

void stand_reserveVirtualRange(void* start, size_t len, 
				 unsigned short flags, const char* tag) {
  assert(len);

  int i;
  for(i=0; i<stand_bootContext()->virtual_map.count; i++) {
    if(boot_context->virtual_map.segments[i].base == start)
      panic("DOUBLE reserveVirtualRange at 0x%x len 0x%x '%s'", start, len, tag);
  }
  

  struct stand_virtual_segment *vm = 
    &boot_context->virtual_map.segments[stand_bootContext()->virtual_map.count++];
  vm->base = start;
  vm->length = len;
  vm->flags = flags;
  vm->tag = tag;

  stand_virtualSegmentSortedInsert(boot_context, vm);

  if(stand_bootContext()->virtual_map.count > 80)
    panic("overflowing libstand virtual map!");
}

/** searches through the list of virtual 
 * memory segments and returns the first available unallocated
 * space of at least "pages" pages starting from address "_start".
 *  it assumes that the segment list is ordered by address */

void* stand_getNextFreeVirtual(void* _start, size_t pages) {
  assert(pages);

  pointer_t start = (pointer_t)_start;
  pointer_t end = start + pages * PAGE_SIZE;
  struct stand_virtual_memory_map* map = &boot_context->virtual_map;
  struct stand_virtual_segment* segment;

  //  printf("BEGIN SEARCH 0x%x:0x%x (%x pages)\n", start, end, pages);
  list_head_t *pos;
  list_for_each(pos, &map->ordered) {
    segment = list_entry(pos, struct stand_virtual_segment, list);
    pointer_t sbase = (pointer_t)segment->base; 
    pointer_t send = sbase + segment->length*PAGE_SIZE; 

    //    printf("SEARCHING SEGMENT 0x%x->0x%x in 0x%x->0x%x ", start, end, sbase, send);

    if(send < start) {
      //      printf("\n");
      continue;
    }   

    if(((sbase <= start) && (start < send)) ||
       ((sbase <= end) && (end < send))) {
      //      printf("conflict\n");
      start = send;
      end = start + pages * PAGE_SIZE;
      if(end < start) {
	panic("WRAPPING AROUND");
      }
      continue;
    }

    //    printf("no conflict\n");

    //    break;
  }
  //  printf("FOR EXAUSTED, GOT FREE SEGMENT 0x%x\n", start);
  return (void*)start;
}

struct stand_virtual_segment* stand_findVirtualSegment(char* name) {
  struct stand_virtual_memory_map* map = &boot_context->virtual_map;
  int i=0;
  for(i=0; i<map->count; i++) {
    if(strcmp(map->segments[i].tag, name) == 0)
      return &map->segments[i];
  }
  return 0;
}
