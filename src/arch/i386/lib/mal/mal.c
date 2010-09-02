#include <mal/mal.h>
#include <platform/mal/gdt.h>
#include <platform/mal/idt.h>

void mal_init() {
  mal_gdtInit();
  mal_idtInit();
}
