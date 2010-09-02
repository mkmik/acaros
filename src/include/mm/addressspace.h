#ifndef _MM_ADDRESSSPACE_H_
#define _MM_ADDRESSSPACE_H_

/** may read */
#define PROT_READ_BIT  0
/** may write */
#define PROT_WRITE_BIT 1
/** copy on write */
#define PROT_COW_BIT   2
/** user-space may access */
#define PROT_USER_BIT  3
#define PROT_READ      (1<<PROT_READ_BIT)
#define PROT_WRITE     (1<<PROT_WRITE_BIT)
#define PROT_COW       (1<<PROT_COW_BIT)
#define PROT_USER      (1<<PROT_USER_BIT)

/** initializes system address space */
void mm_addressspace_init();
/** maps the initial framebuffer for debugging */
void mm_initFramebuffer();

#endif
