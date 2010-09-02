#ifndef _BOOT_CONTEXT_H_
#define _BOOT_CONTEXT_H_

#include "loader.h"
#include <stand/stand.h>

extern struct stand_boot_context boot_context;
extern struct stand_module modules[LOADER_MAX_MODULES];
extern struct stand_physical_segment segments[LOADER_MAX_SEGMENTS];
extern struct stand_physical_segment allocatedSegments[LOADER_MAX_SEGMENTS];
extern struct stand_virtual_segment vsegments[LOADER_MAX_SEGMENTS];

void init_boot_context();
struct stand_boot_context* virtual_boot_context();
void fill_boot_context();

#endif
