#ifndef _PLATFORM_INITIAL_PAGING_H_
#define _PLATFORM_INITIAL_PAGING_H_

#include <mm/addressspace.h>
#include <platform/x86.h>
#include <platform/paging.h>
//#include <hal.h>

//#define HYPERSPACE 0xf0000000
#define HYPERSPACE 0xffc00000

#define PTE_PSE      0x80 // 4MB page
#define PTE_USER     0x4
#define PTE_RW       0x2
#define PTE_PRESENT  0x1

/** initialize identity paging of first 4MB of physical memory */
int hat_initInitialPaging();

/** page aligned buffer containing page directory entries (pte_t)
 * which points to page tables */
extern pte_t *page_directory;
/** page table for the identity mapping of the first 4M of physical memory */
extern pte_t *identity_page_table;

/** sets the pte to null */
void hat_resetPte(pte_t *pte);
/** modify the physical address field in the pte */
void hat_setPteBase(pte_t *pte, void *base);
/** modify the flags of the pte  */
void hat_setPteFlags(pte_t *pte, unsigned short flags);
/** get the physical address of the page pointed by the pte */
void *hat_getPteBase(pte_t *pte);
/** get the pte flags */
unsigned short hat_getPteFlags(pte_t *pte);

#define PAGE_TABLE(addr) ((pte_t*)(HYPERSPACE + hat_pageDirIndex(addr) * PAGE_SIZE))
#define PAGE_DIRECTORY ((pte_t*)(HYPERSPACE + hat_pageDirIndex(HYPERSPACE) * PAGE_SIZE))

/** extract the page directory index of addr */
#define hat_pageDirIndex(addr) ((u_int32_t)(addr) >> 22)
/** extract the page table index of addr */
#define hat_pageTableIndex(addr) (((u_int32_t)(addr) >> 12) & 0x3FF)

extern void* (*hat_initial_get_next_physical_page)();

/** returns true if the page is valid */
int hat_isValid(pointer_t addr);

#endif
