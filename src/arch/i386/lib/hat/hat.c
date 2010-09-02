#include <hat/hat.h>
#include <hat/initial_paging.h>
#include <platform/hat/initial_paging.h>

#include <log/log.h>

pointer_t hat_virtualToPhysical(void* ptr) {
  pte_t* dir = PAGE_DIRECTORY;

  if(!(hat_getPteFlags(&dir[hat_pageDirIndex(ptr)]) & PTE_PRESENT))
    return -1;
  
  pte_t* table = PAGE_TABLE(ptr);
  //  return (pointer_t)hat_getPteBase(&table[hat_pageTableIndex(ptr)]);
  pointer_t page = (pointer_t)hat_getPteBase(&table[hat_pageTableIndex(ptr)]);
  return page + ((pointer_t)ptr & 0xFFF);
}

int hat_isPresent(void* ptr) {
  pte_t* dir = PAGE_DIRECTORY;
  
  if(!(hat_getPteFlags(&dir[hat_pageDirIndex(ptr)]) & PTE_PRESENT))
    return 0;
  
  pte_t* table = PAGE_TABLE(ptr);
  return hat_getPteFlags(&table[hat_pageTableIndex(ptr)]) & PTE_PRESENT;
}

int mm_pageFrameNumber(pointer_t ptr) {
  return ptr / PAGE_SIZE;
}

int hat_sizeToPages(size_t s) {
  if(s == 0)
    return 0;
  return (s-1) / PAGE_SIZE + 1;
}
