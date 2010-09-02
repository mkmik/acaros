#ifndef _STAND_H_
#define _STAND_H_

#include <types.h>
#include <list.h>

struct stand_module {
  char* name;
  void* data;
  size_t size;
};

struct stand_module_map {
  int count;
  struct stand_module* modules;
};

typedef enum {RESERVED, RAM} stand_physical_type_t;

struct stand_physical_segment {
  void *base;
  size_t length;
  stand_physical_type_t type;
};

struct stand_physical_memory_map {
  int count;
  struct stand_physical_segment* segments;
  int allocatedCount;
  struct stand_physical_segment* allocatedSegments;
};

struct stand_virtual_segment {
  void *base;
  size_t length;
  unsigned short flags;
  list_head_t list;
  const char* tag;
};

struct stand_virtual_memory_map {
  int count;
  struct stand_virtual_segment* segments;
  /** orderd list of allocated segments */
  list_head_t ordered;
};


struct stand_boot_context {
  struct stand_physical_memory_map memory_map;
  struct stand_module_map modules;
  struct stand_virtual_memory_map virtual_map;
  const char* cmdline;
  void* kernelBase;
};

/** initializes the stand subsystem */
void stand_init(struct stand_boot_context* ctx);

/** prints intro screen. 
 * clears the screen and pretty prints the OS name and version */
void stand_printIntro();

/** find the a module by name. If no such module 
 * exists 0 is returned */
void* stand_findModule(char* name);

struct stand_physical_segment* stand_getRamSegment(int idx);

struct stand_boot_context* stand_bootContext();

void stand_virtualSegmentSortedInsert(struct stand_boot_context *context,
				      struct stand_virtual_segment *vs);

void stand_reserveVirtualRange(void* start, size_t len, 
				 unsigned short flags, const char* tag);

/** find the first free address after the segment starting with @start */
void* stand_getNextFreeVirtual(void* start, size_t pages);
/** return 0 if not found */
struct stand_virtual_segment* stand_findVirtualSegment(char* name);

#endif
