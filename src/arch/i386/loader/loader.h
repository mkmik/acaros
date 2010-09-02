#ifndef _LOADER_H_
#define _LOADER_H_

#define LOADER

#define LOADER_MAX_MODULES 512
#define LOADER_MAX_SEGMENTS 81

#define LOADER_INIT_STACK_SIZE (8*1024)

struct stand_boot_context;

typedef void (*entry_t)(struct stand_boot_context* ctx);

entry_t load_elf(void* buffer);

/** creates and maps the inital stack.
 * returns the virtual address of the top of the stack */
void* createInitStack();

/** switches the stack and calls the entry point */
void callWithNewStack(entry_t entry, void* arg, void* newStack);

#endif
