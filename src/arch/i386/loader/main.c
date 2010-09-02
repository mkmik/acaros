#include <autoconf.h>
#include <version.h>
#include "loader.h"
#include <stand/libc.h>
#include <stand/stand.h>
#include "multiboot.h"
#include "boot_context.h"

#include <platform/hat/page_allocator.h>
#include <platform/hat/initial_paging.h>
#include <hat/initial_paging.h>
#include <elf.h>
#include <log/log.h>

void panic(const char* msg, ...) {
  printf("\n\n");
  printf("loader panic: %s", msg);
  while(1);
}

void klogf(log_level_t level, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  dvprintf(&libc_video_ops, fmt, args);
}

void start(u_int32_t magic, struct multiboot_info *info) {
  mb_magic = magic;
  mb_info = info;

  stand_printIntro();
  printf("loading Acaros kernel ... ");

  init_boot_context();
  fill_boot_context();
  stand_init(&boot_context);
  
  init_page_allocator();
  hat_initInitialPaging();

  void* module = stand_findModule("kernel");
  if(module == 0)
    panic("cannot find kernel image in archive");

  entry_t entry = load_elf(module);
  if(entry) 
    printf("ok\n");
  else
    panic("cannot load kernel image");

  void* stack = createInitStack();
  callWithNewStack(entry, virtual_boot_context(), stack);

  panic("kernel returned to loader");
}

