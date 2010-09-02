#include "video.h"
#include <stand/libc.h>
#include <mal/ioport.h>

#define PORT 0x3d4
#define DEFAULT_FRAMEBUFFER ((char*)0xB8000)
#define COLUMNS 80
#define ROWS 25

static int columns = COLUMNS, rows = ROWS;
static int attr = 0x17;

static char* framebuffer = DEFAULT_FRAMEBUFFER;

void video_setFramebufferBase(void* addr) {
  framebuffer = (char*)addr;
}

void video_init() {
}

void video_set_cursor_pos(int row, int col) {
  int off = col + row * columns;
  int port = 0x3d4;

  mal_outb_p(0xe, port + 0);
  mal_outb_p(off >> 8, port + 1);
  mal_outb_p(0xf, port + 0);
  mal_outb_p(off & 0xFF, port + 1);
}

void video_set_screen_size(int r, int c) {
  rows = r;
  columns = c;
}

void video_drawAA(char *s, int row, int col, size_t len) {
  char *p=s;
    
  while(len--) {
    if(*p == '\n') {
      row++;
      col=0;
    } else 
      if(col < columns && row < rows) {
//	if(*p != ' ') 
	  video_draw_char(*p, row, col++);
//	else col++;
      }
    p++;
  }
}

void video_draw_char(char c, int row, int col) {
  *(framebuffer + (col + row * columns) * 2) = (char)(c & 0xFF);
  *(framebuffer + (col + row * columns) * 2 + 1) = attr;   
  video_set_cursor_pos(row, col+1);
}

void video_vscroll(int delta) {
  int row;
  int col;

  for(row=delta; row<rows; row++) {
    for(col=0; col<columns; col++) {
      *(framebuffer + (col + (row-delta) * columns) * 2) = 
	*(framebuffer + (col + row * columns) * 2);
      *(framebuffer + (col + (row-delta) * columns) * 2 + 1) = 
	*(framebuffer + (col + row * columns) * 2 + 1) ;
    }
  }
  for(row=rows-delta; row<rows; row++) {
    for(col=0; col<columns; col++) {
      *(framebuffer + (col + row * columns) * 2) = ' ';
      *(framebuffer + (col + row * columns) * 2 + 1) = attr;
    }
  }
}

void setAttr(char a) {
  attr = a;
}

char getAttr() {
  return attr;
}

void video_cls() {
  int i;
  for(i=0; i<rows*columns * 2 ; i+=2) {
    framebuffer[i] = ' ';
    framebuffer[i+1] = attr;
  }
}

int video_get_rows() {
  return rows;
}

int video_get_columns() {
  return columns;
}
