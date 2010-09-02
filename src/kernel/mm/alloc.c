#include <mm/alloc.h>
#include <log/log.h>

#define POOL_SIZE (4096*12)

char malloc_pool[POOL_SIZE];

static char* alloc_base = malloc_pool;

void* mm_malloc(size_t size) {
  void *res = alloc_base;
  alloc_base += size;
  if(alloc_base >= malloc_pool + POOL_SIZE)
    panic("malloc pool full");
  return res;
}
