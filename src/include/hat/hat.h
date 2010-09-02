#ifndef _HAT_H_
#define _HAT_H_

#include <types.h>

/** transforms a virtual address to a physical */
pointer_t hat_virtualToPhysical(void* ptr);
/** returns true if the page is present in physical memory */
int hat_isPresent(void* ptr);
void* hat_physical_to_virtual(pointer_t ptr);

/** 
 * returns the page frame number associated with an address
 **/
int mm_pageFrameNumber(pointer_t ptr);

/** returns an integer number of pages with can
 * accomodate s bytes */
int hat_sizeToPages(size_t s);

#endif
