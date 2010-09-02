#ifndef _PFD_H_
#define _PFD_H_

#include <types.h>
#include <list.h>

typedef struct {
  list_head_t link;
} page_frame_t;

typedef struct {
  list_head_t free_list;
  list_head_t used_list;
  list_head_t bad_list;
  int numFrames;
  page_frame_t* frames;
} page_frame_database_t;

void mm_initPfd();
extern page_frame_database_t pfd;

#define mm_pfd_setFrameState(frame,state) { \
 list_del(&pfd.frames[frame].link); \
 list_add(&pfd.frames[frame].link, &pfd. state ## _list); \
} while(0)

void* mm_allocPhysicalPage();

/** alloc from bottom of physical memory
 * useful until we have not a zoned physical memory allocator
 * in order to deal with dma memory */
void* mm_allocPhysicalPageBottom();
#endif
