#ifndef _SIMPLOS_LIBC_H_
#define _SIMPLOS_LIBC_H_

#include <types.h>
#include <stdarg.h>
#include <string.h>
//#include <video.h>

/** this is a primitive way to directing
 * output to different devices within the kernel.
 * it will be changed soon*/
struct libc_dev_ops {
  int (*f_putchar)(int);
  int (*f_getchar)();
};

extern struct libc_dev_ops libc_video_ops;
extern struct libc_dev_ops libc_serial_ops;

int dputchar(struct libc_dev_ops* ops, int c);
int dputs(struct libc_dev_ops* ops, const char* string);
int dprintf(struct libc_dev_ops* ops, const char* format,...);
int dvprintf(struct libc_dev_ops* ops, const char* format, va_list args);

#define putchar(ch) dputchar(&libc_video_ops, ch)
#define puts(s) dputchar(&libc_video_ops, s)
#define printf(args...) dprintf(&libc_video_ops, args)
#define vprintf(format, args) dvprintf(&libc_video_ops, format, args)

void cls();
void itoa (char *buf, int base, int d);
unsigned char bcd2int(unsigned char bcd);

void setPos(int row, int col);
int getRow();
int getCol();

#endif
