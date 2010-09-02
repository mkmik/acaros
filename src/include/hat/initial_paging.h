#ifndef _INITIAL_PAGING_H_
#define _INITIAL_PAGING_H_

#include <types.h>

/** maps a page range to the given virtual address addr
 * allocating physical memory. The length is given in pages */
int hat_initialMapAnonPages(pointer_t addr, size_t len,
			    unsigned short flags, const char* tag);

/** maps a page range to the given virtual address addr, mapping it
 * to the specified physical memory. The length is given in pages  */
int hat_initialMapPages(pointer_t addr, pointer_t phy, size_t len, 
			unsigned short flags, const char* tag);

/** unmaps first 4MB of physical memory */
int hat_unmapIdentityPages();

#endif
