#include <platform/hat/initial_paging.h>
#include <platform/hat/page_allocator.h>
#include <mm/addressspace.h>
#include <stand/libc.h>
#include <stand/stand.h>
#include <debug.h>

void* (*hat_initial_get_next_physical_page)() = get_next_physical_page;

pte_t *page_directory;
/** first physical 4Mb */
pte_t *identity_page_table;

void hat_resetPte(pte_t *pte) {
  pte->data = 0;
}

void hat_setPteBase(pte_t *pte, void *base) {
  pte->data &= ~(0xFFF);
  pte->data |= (u_int32_t)base & ~(0xFFF);
}

void hat_setPteFlags(pte_t *pte, unsigned short flags) {
  pte->data &= ~(0x1FF);
  pte->data |= flags;
}

void *hat_getPteBase(pte_t *pte) {
  return (void*)(pte->data & ~(0xFFF));
}

unsigned short hat_getPteFlags(pte_t *pte) {
  return pte->data & (0x1FF);
}

int hat_initInitialPaging() {
  page_directory = (pte_t*)(get_next_physical_page());
  identity_page_table = (pte_t*)(get_next_physical_page());

  memset(page_directory, PAGE_SIZE, 0);  
  memset(identity_page_table, PAGE_SIZE, 0);  

  hat_setPteBase(&page_directory[0], identity_page_table);
  hat_setPteFlags(&page_directory[0], PTE_RW | PTE_PRESENT | PTE_USER);

  hat_setPteBase(&page_directory[hat_pageDirIndex(HYPERSPACE)], page_directory);
  hat_setPteFlags(&page_directory[hat_pageDirIndex(HYPERSPACE)], PTE_RW | PTE_PRESENT | PTE_USER);
  
  int i;
  for(i=0; i<1024; i++) {
    hat_setPteBase(&identity_page_table[i], (void*)(i * PAGE_SIZE));
    hat_setPteFlags(&identity_page_table[i], PTE_RW | PTE_PRESENT);
  }

  WRITE_PAGE_DIRECTORY(page_directory);
  ENABLE_PAGING(); 

  stand_reserveVirtualRange((void*)PAGE_DIRECTORY, 1, 0, "hyperspace dir");
  stand_reserveVirtualRange((void*)PAGE_TABLE(0), 1, 0, "intentity pagetable");
  //  stand_reserveVirtualRange((void*)HYPERSPACE, 1024, 0, "hyperspace");

  return 0;
}

int hat_unmapIdentityPages() {
  pte_t *page_table;

  page_table = PAGE_TABLE(0);

  int i;
  //  for(i=0; i<1024; i++) 
  for(i=0; i<1024; i++) 
    hat_setPteFlags(&page_table[i], 0);
  //hat_setPteFlags(&(PAGE_DIRECTORY[0]), 0);

  flush_tlb();

  return 0;
}

int hat_initialMapAnonPages(pointer_t addr, size_t len, 
			    unsigned short flags, const char* tag) {
  int dir;
  int tab;
  pte_t *page_table;

  assert(len);

  // reserve if not already reserved
  pointer_t oldReservation = (pointer_t)stand_getNextFreeVirtual((void*)addr, len);
  if(oldReservation == addr)
    stand_reserveVirtualRange((void*)addr, len, flags, tag); 

  int pflags = 0;
  if(flags & PROT_WRITE)
    pflags |= PTE_RW;
  if(flags & PROT_USER)
    pflags |= PTE_USER;

  while(len--) {
    dir = hat_pageDirIndex(addr);
    tab = hat_pageTableIndex(addr);

    if(!hat_getPteBase(&PAGE_DIRECTORY[dir])) {
      pte_t* new_page_table;
      new_page_table = hat_initial_get_next_physical_page();
      if(new_page_table == (void*)-1)
	return 0;
      stand_reserveVirtualRange((void*)PAGE_TABLE(addr), 1, 0, "hyperspace table");

      hat_setPteBase(&(PAGE_DIRECTORY[dir]), new_page_table);
      hat_setPteFlags(&(PAGE_DIRECTORY[dir]), PTE_RW | PTE_PRESENT | pflags);
      memset(PAGE_TABLE(addr), PAGE_SIZE, 0);
    } 

    page_table = PAGE_TABLE(addr);
    void* page = hat_initial_get_next_physical_page();
    if(page == (void*)-1)
      return 0;
    hat_setPteBase(&page_table[tab], page);
    hat_setPteFlags(&page_table[tab], pflags | PTE_PRESENT);
    
    addr += PAGE_SIZE;
  }

  flush_tlb();

  return 1;
}


/** TODO share implementation with hat_mapAnonPages */
int hat_initialMapPages(pointer_t addr, pointer_t phy, size_t len, 
			unsigned short flags, const char* tag) {
  int dir;
  int tab;
  pte_t *page_table;
 
  // reserve if not already reserved
  pointer_t oldReservation = (pointer_t)stand_getNextFreeVirtual((void*)addr, 1);
  if(oldReservation == addr)
    stand_reserveVirtualRange((void*)addr, len, flags, tag);

  int pflags = 0;
  if(flags & PROT_WRITE)
    pflags |= PTE_RW;
  if(flags & PROT_USER)
    pflags |= PTE_USER;

  while(len--) {
    dir = hat_pageDirIndex(addr);
    tab = hat_pageTableIndex(addr);

    if(!hat_getPteBase(&PAGE_DIRECTORY[dir])) {
      pte_t* new_page_table;
      new_page_table = hat_initial_get_next_physical_page();
      if(new_page_table == 0)
	return 0;
      stand_reserveVirtualRange((void*)PAGE_TABLE(addr), 1, 0, "hyperspace table");

      hat_setPteBase(&(PAGE_DIRECTORY[dir]), new_page_table);
      hat_setPteFlags(&(PAGE_DIRECTORY[dir]), PTE_RW | PTE_PRESENT | PTE_USER);
      memset(PAGE_TABLE(addr), PAGE_SIZE, 0);
    } 

    page_table = PAGE_TABLE(addr);
    hat_setPteBase(&page_table[tab], (void*)phy);
    hat_setPteFlags(&page_table[tab], PTE_RW | PTE_PRESENT | PTE_USER);
    
    addr += PAGE_SIZE;
    phy += PAGE_SIZE;
  }

  flush_tlb();

  return 1;
}

int hat_isValid(pointer_t addr) {
  if(hat_getPteFlags(&PAGE_DIRECTORY[hat_pageDirIndex(addr)]) & 0x1)
    if(hat_getPteFlags(&PAGE_TABLE(addr)[hat_pageTableIndex(addr)]) & 0x1)
      return 1;
  return 0;
}
