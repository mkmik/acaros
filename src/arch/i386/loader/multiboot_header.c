#include <multiboot.h>

#define MULTIBOOT_HEADER_FLAGS (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)

const struct multiboot_header mbhead __attribute__((section(".text"))) = {
  MULTIBOOT_HEADER_MAGIC,
  MULTIBOOT_HEADER_FLAGS,
  -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS), // checksum
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x1,
  0x0,
  0x0,
  0x0
};
