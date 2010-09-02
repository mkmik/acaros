#ifndef _FILTER_H_
#define _FILTER_H_

void coc_filter(const char* source, const char* dest,
		const char* includePaths);

typedef enum {BODY=0, ESCAPE, CLASS, CLASSBODY, DEF, VAR, CALL, PREPROC} coc_mode_t;

struct state {
  void (*process)(char ch, coc_mode_t *next_mode);
  void (*change)();
};

void body_process(char ch, coc_mode_t *next_mode);
void escape_process(char ch, coc_mode_t *next_mode);
void class_process(char ch, coc_mode_t *next_mode);
void classbody_process(char ch, coc_mode_t *next_mode);
void def_process(char ch, coc_mode_t *next_mode);
void var_process(char ch, coc_mode_t *next_mode);
void call_process(char ch, coc_mode_t *next_mode);
void preproc_process(char ch, coc_mode_t *next_mode);

void class_change();
void classbody_change();
void def_change();
void var_change();
void call_change();
void preproc_change();

#endif
