#include <types.h>
#include <stand/libc.h>
#include "tar.h"

int tar_size(struct tar_header* header) {
  char *size = header->size;
  int i=11, s=0;
  while(i--)
    s = s*8 + (*size++-'0') ;
  return s;
}

int tar_file_size(struct tar_archive* file) {
  return tar_size(&file->block[0].header);
}

int tar_blocks_from_size(int size) {
  int blocks = size / BLOCKSIZE;
  if((blocks * BLOCKSIZE) < size)
    blocks++;
  return blocks;
}

int tar_file_blocks(struct tar_archive* file) {
  return tar_blocks_from_size(tar_size(&file->block[0].header));
}

void *tar_file_data(struct tar_archive* file) {
  return (void*)&file->block[1];
}

struct tar_archive* tar_next_file(struct tar_archive* arch) {
  return (struct tar_archive*)&arch->block[tar_file_blocks(arch)+1];
}

int tar_is_valid_file(struct tar_archive* file) {
  return strncmp(file->block[0].header.magic, "ustar", 5) == 0;
}

struct tar_archive* tar_find_file(struct tar_archive* arch, char* name) {
  while(tar_is_valid_file(arch)) {
    if(strcmp(arch->block[0].header.name, name) == 0)
      return arch;
    arch = tar_next_file(arch);
  }
  return 0;
}
/*
static void print_file(struct tar_archive* file) {
  int size,i;
  struct tar_header *header = &file->block[0].header;
  char *data;
  
  size = tar_size(header);
  data = file->block[1].buffer;

  i=size;
  while(i--)
  uputchar(*data++);

}
*/

/*void tar_test(struct tar_archive* arch, char *name) {
  struct tar_archive *oldarch;
    
  oldarch = arch;
  if(!arch) {
    uprintf("invalid tar archive\n");
    return;
  }
  
  uprintf("reading tar from %p...\n", arch);
  while(tar_is_valid_file(arch)) {
    uprintf(" %s", arch->block[0].header.name);
    if(arch->block[0].header.typeflag == DIRTYPE)
      uprintf(" (DIR)");
    uprintf("\n");
    arch = tar_next_file(arch);
  }
  uprintf("done...\n\n");

  arch = tar_find_file(oldarch, name);
  if(arch)
    print_file(arch);
  else
    uprintf("no such file '%s'\n", name);  
}
*/
