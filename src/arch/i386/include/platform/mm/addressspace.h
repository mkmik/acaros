#ifndef _PLATFORM_ADDRESSSPACE_H_
#define _PLATFROM_ADDRESSSPACE_H_

/** initialize the platform dependent address space routines.
 * installs the page fault handler */
void mm_platform_addressspace_init();

#endif
