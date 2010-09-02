#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <types.h>

void video_init();
void video_setFramebufferBase(void* base);
void video_drawAA(char *s, int row, int col, size_t len);
void video_draw_char(char c, int row, int col);
void video_set_cursor_pos(int row, int col);
void video_set_screen_size(int r, int c);

void video_vscroll(int rows);

void setAttr(char attr);
char getAttr();

int video_get_rows();
int video_get_columns();
void video_cls();

#endif
