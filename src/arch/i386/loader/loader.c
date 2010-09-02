#include "loader.h"
#include <hat/hat.h>
#include <platform/hat/initial_paging.h>
#include <hat/initial_paging.h>
//#include "page_allocator.h"
#include <stand/libc.h>
#include <stand/stand.h>
#include <elf.h>

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(arg, ...) do { } while(0)
//#define DEBUG(arg, ...) printf(arg, ...)

entry_t load_elf(void* buffer) {
  struct elf32_hdr* elf_header;
  struct elf32_phdr* prog_header;
  int i;

  elf_header = (struct elf32_hdr*)buffer;
  DEBUG("\ntype: %p\n", elf_header->e_type);
  DEBUG("machine: %p\n", elf_header->e_machine);
  DEBUG("entry: %p\n", elf_header->e_entry);
  DEBUG("e_phentsize: %p\n", elf_header->e_phentsize);
  DEBUG("e_phnum: %p\n", elf_header->e_phnum);
  DEBUG("e_phoff: %p\n", elf_header->e_phoff);

  prog_header = (struct elf32_phdr*)((char*)elf_header + elf_header->e_phoff);

  for(i=0; i < elf_header->e_phnum; i++) {
    DEBUG("--- prog header %d ---- \n", i);
    //    DEBUG("type %d\n", prog_header->p_type);
    //    DEBUG("offset %x\n", prog_header->p_offset);
    DEBUG("vaddr %p\n", prog_header->p_vaddr);
    //    DEBUG("filesz %x\n", prog_header->p_filesz);
    DEBUG("memsz %x\n", prog_header->p_memsz);
    DEBUG("flags %c%c%c %x\n", 
	   prog_header->p_flags & PF_R ? 'R' : '-',
	   prog_header->p_flags & PF_W ? 'W' : '-',
	   prog_header->p_flags & PF_X ? 'X' : '-',
	   prog_header->p_flags
	   );

    if(prog_header->p_memsz == 0) {
      printf("vaddr %p\n", prog_header->p_vaddr);
      continue;
    }

    hat_initialMapAnonPages(prog_header->p_vaddr & (~0xFFF),  
			    hat_sizeToPages(prog_header->p_memsz), 
			    PROT_WRITE, "elf section"); // TODO: handle prot
    DEBUG("COPING %d bytes from %p to %p\n", 
	   prog_header->p_filesz, 
	   (char*)elf_header + prog_header->p_offset,
	   prog_header->p_vaddr);
    memcpy((void*)prog_header->p_vaddr, 
	   (char*)elf_header + prog_header->p_offset, prog_header->p_filesz);

    // next
    prog_header = (struct elf32_phdr*)((char*)prog_header + elf_header->e_phentsize);        
  }
  
  stand_bootContext()->kernelBase = (void*)elf_header->e_entry;
  return (void*)elf_header->e_entry;

}

char tmp_init_stack[LOADER_INIT_STACK_SIZE];

void* createInitStack() {
  stand_reserveVirtualRange((void*)(HYPERSPACE - LOADER_INIT_STACK_SIZE),
			    LOADER_INIT_STACK_SIZE / PAGE_SIZE, 0,
			    "init stack");
  hat_initialMapAnonPages(HYPERSPACE - LOADER_INIT_STACK_SIZE,
		  LOADER_INIT_STACK_SIZE / PAGE_SIZE ,
		  PROT_WRITE, "init stack");

  u_int32_t base = HYPERSPACE-LOADER_INIT_STACK_SIZE+2048;

  return (void*)base;
}

void callWithNewStack(entry_t entry, void* arg, void* newStack) {
  asm("mov %0, %%esp\n"
      "push %1\n"
      "call *%2" : : "g"(newStack), "r"(arg), "r"(entry));
}
