#include <stand/libc.h>
#include "video.h"

static int row = 0, col = 0;

int video_putchar(int c);

struct libc_dev_ops libc_video_ops = {
  video_putchar, 0
};

static void newline() {
  col = 0;
  row++;
  if(row >= (video_get_rows())) {
    row--;
    video_vscroll(1);
  }
  video_set_cursor_pos(row, col);
}

void cls() {
  video_cls();
  setPos(0, 0);
}

void itoa (char *buf, int base, int d) {
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
    
  /* If %d is specified and D is minus, put -' in the head. */
  if (base == 'd' && d < 0)
    {
	
      *p++ = '-';
      buf++;
      ud = -d;
    }
    
  else if (base == 'x')
    divisor = 16;
    
  /* Divide UD by DIVISOR until UD == 0. */
  do
    {
	
      int remainder = ud % divisor;
	
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
    
  while (ud /= divisor);
    
  /* Terminate BUF. */
  *p = 0;
    
  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}

int video_putchar(int c) {
  if(c == '\n' || c == '\r') 
    newline();
  else {
    video_draw_char(c, row, col);
    
    col++;
    if(col >= video_get_columns())
      newline();
  }
  return c;
}

int dputchar(struct libc_dev_ops* ops, int c) {
  if(ops->f_putchar)
    return ops->f_putchar(c);
  return -1;
}

int dputs(struct libc_dev_ops* ops, const char *str) {
  const char *s = str;
  while(*s)
    dputchar(ops, *s++);
  dputchar(ops, '\n');
  return s-str;
}

int dprintf(struct libc_dev_ops* ops, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int res = dvprintf(ops, format, args);
  va_end(args);
  return res;
}

int dvprintf(struct libc_dev_ops* ops, const char *format, va_list args) {
  //  char **arg = (char **) &format;
  int c;
  char buf[20];
    
  //  arg++;
    
  while ((c = *format++) != 0)
    {
	
      if (c != '%')
	dputchar (ops, c);
      else
	{
	  int arg_int;
	  char* arg_str;
	    
	  c = *format++;
	  switch (c)
	    {
		
	    case 'p':
	      dputchar(ops, '0');
	      dputchar(ops, 'x');
	      c = 'x';
	      // fall though
	    case 'd':
	    case 'u':
	    case 'x':
	      arg_int = va_arg(args, int);
	      itoa (buf, c, arg_int);
	      arg_str = buf;
	      goto string;
	      break;
	      
		
	    case 's':
	      arg_str = va_arg(args, char*);
	      if (! arg_str)
		arg_str = "(null)";
		
	    string:
	      while (*arg_str)
		dputchar (ops, *arg_str++);
	      break;
		
	    default:
	      arg_int = va_arg(args, int);
	      dputchar (ops, arg_int);
	      break;
	    }
	}
    }
  return 0;
}

void setPos(int r, int c) {
  row = r;
  col = c;
}

int getRow() {
  return row;
}

int getCol() {
  return col;
}

unsigned char bcd2int(unsigned char bcd) {
  return (bcd >> 4) * 10 + (bcd & 0xF);
}

int strcmp(const char* s1, const char*s2) {
  while(*s1 && *s2) {
    if(*s1 != *s2)
      return *s2 - *s1;
    else
      s1++, s2++;
  }
  
  if(*s1 == *s2)
    return 0;
  else
    return *s1 ? 1 : -1;
}

int strncmp(const char* s1, const char*s2, size_t size) {
  while(*s1 && *s2 && size--) {
    if(*s1 != *s2)
      return *s2 - *s1;
    else
      s1++, s2++;
  }
  
  if(*s1 == *s2 || size == 0)
    return 0;
  else
    return *s1 ? 1 : -1;
}

size_t strlen(const char *s) {
  int i=0;
  while(*s++) 
    i++;
  return i;
}

char *strstr(const char *haystack, const char *needle) {
  /** I don't have time to think ...*/
  int hi = strlen(haystack);
  int len = strlen(needle);
  int i;
  for(i=0; i < hi; i++)
    if(strncmp(haystack+i, needle, len) == 0)
      return (char*)(haystack+i);
  return 0;
}

void *memcpy (void *__restrict dest,
	      __const void *__restrict src, size_t n) {
  void *d=dest;
  while(n--)
    *((char*)dest)++ = *((char*)src)++;
  return d;
}

void *memset (void *s, int c, size_t n) {
  while(n--)
    *((char*)s)++ = c;
  return s;
}
