#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algo/avl.h>
#include "filter.h"
#include "def.h"
#include "class.h"
#include "dir.h"

FILE* in;
FILE* out;
unsigned int lineno;

#define BUFFER_SIZE (4096)
#define CLASS_NAME_SIZE (4096)

struct state states[] = {
  {body_process, 0},
  {escape_process, 0},
  {class_process, class_change},
  {classbody_process, classbody_change},
  {def_process, def_change},
  {var_process, var_change},
  {call_process, call_change},
  {preproc_process, preproc_change}
};

int isseparator(char ch) {
  return strchr(" \t\n=*+-/()[]{}!~|&^\"';:,.", ch) != 0;
}

char ahead_buffer[BUFFER_SIZE];

void append(char ch, char* buffer) {
  strncat(buffer, &ch, 1);
  ch = 0;
  strncat(buffer, &ch, 1);
  if(strlen(buffer) >= BUFFER_SIZE) 
    fprintf(stderr, "ERROR\n");
}

void reset_buffer(char* buffer) {
  buffer[0] = 0;
}

void body_process(char ch, coc_mode_t *next_mode) {
  if(ch == '@') {
    *next_mode = ESCAPE;
  } else if(ch == '#') {
    *next_mode = PREPROC;
  } else if(isseparator(ch)) {
    fprintf(out, "%s%c", ahead_buffer, ch);
    reset_buffer(ahead_buffer);
  } else
    append(ch, ahead_buffer);
}

void escape_process(char ch, coc_mode_t *next_mode) {
  if(ch == '+') {
    *next_mode = CLASS;
    reset_buffer(ahead_buffer);
  } else if(ch == '-') {
    *next_mode = DEF;
    reset_buffer(ahead_buffer);
  } else {
    *next_mode = VAR;
    ungetc(ch, in);
  }
}

static char className[CLASS_NAME_SIZE] = "";
static char superclassName[CLASS_NAME_SIZE] = "";

void class_process(char ch, coc_mode_t *next_mode) {

  if(ch == '{') {
    const char* class = class_parse_className(ahead_buffer);
    const char* superclass = class_parse_superclassName(ahead_buffer);
    if(strcmp(class, "Class")) { // exception: Class_t is predefined 
      fprintf(out, "typedef struct %s_s %s_t;\n", class, class);
    }
    fprintf(out, "struct %s_s {\n", class);
    // new gcc doesn't support this trick -> remember definitions in classbody state
    //    fprintf(out, "  %s_t;\n", superclass);
    reset_buffer(ahead_buffer);
    
    strcpy(className, class);
    strcpy(superclassName, superclass);
    *next_mode = CLASSBODY;
  } else
    append(ch, ahead_buffer);
}

void class_change() {
  //  fprintf(out, "typedef struct {\n");
}

avl_node_t *classCacheRoot = NULL;

typedef struct ClassCacheEntry{
  char* className;
  struct ClassCacheEntry* superclass;
  char* buffer;
  avl_node_t node;
} ClassCacheEntry;
avl_make_string_compare(ClassCacheEntry, node, className);
avl_make_string_match(ClassCacheEntry, className);

char class_body_buffer[BUFFER_SIZE];

void classbody_process(char ch, coc_mode_t *next_mode) {
  if(ch == '}') {
    fprintf(out, "%s%c", ahead_buffer, ch);
    fprintf(out, ";\n"
	    "Class_t* %s;\n"
	    "const co_class_initializer_t _%s_co_init "
	    "__attribute__((section(\".co.classes\"),weak)) = {\n"
	    "\"%s\", \"%s\", sizeof(%s_t), &%s\n"
	    "}", className, className, className, superclassName, 
	    className, className);
    
    // store class body in avl
    ClassCacheEntry *cacheEntry = malloc(sizeof(ClassCacheEntry));
    cacheEntry->className = strdup(className);
    cacheEntry->superclass = avl_find(superclassName, classCacheRoot, ClassCacheEntry, node);
    cacheEntry->buffer = strdup(class_body_buffer);

    avl_add(&cacheEntry->node, &classCacheRoot, ClassCacheEntry);

    // finishing
    reset_buffer(ahead_buffer);
    *next_mode = BODY;
    className[0] = 0;

  } else {
    append(ch, ahead_buffer);
    append(ch, class_body_buffer);
  }
}

void classbody_outputbody(ClassCacheEntry* class) {
  if(class->superclass)
    classbody_outputbody(class->superclass);
  fprintf(out, "%s", class->buffer);  
}

void classbody_change() {
  reset_buffer(class_body_buffer);
  
  ClassCacheEntry *super = avl_find(superclassName, classCacheRoot, ClassCacheEntry, node);
  if(super)
    classbody_outputbody(super);
}

const char* currentClassName = 0;

void def_process(char ch, coc_mode_t *next_mode) {

  if(ch == '{') {
    const char* className = def_parse_className(ahead_buffer);
    const char* methodName = def_parse_methodName(ahead_buffer);
    char type[512];
    char name[512];
    const char* next_arg;

    currentClassName = className;

    fprintf(out, "object_t * %s_%s(%s_t* self, va_list _args);\n",
            className, methodName, className);

    fprintf(out, 
            "const co_method_initializer_t _%s_%s_co_init "
            "__attribute__((section(\".co.methods\"))) = {\n"
            "\"%s\", \"%s\", (method_t)%s_%s\n"
            "};\n", className, methodName, className, methodName,
	    className, methodName);

    fprintf(out, "object_t * %s_%s(%s_t* self, va_list _args) {\n",
	    className, methodName, className);

    next_arg = ahead_buffer;
    while(*next_arg++ != '(');
    while(isspace(*next_arg))
      next_arg++;
    if(*next_arg == ')')
      next_arg = 0;
    
    while(next_arg) {
      next_arg = def_parse_arg(next_arg, type, name);
      fprintf(out, "  %s %s = va_arg(_args, %s);\n", type, name, type );
    }
    
    fprintf(out, "#line %d", lineno-1);
    reset_buffer(ahead_buffer);
    *next_mode = BODY;
  } else
    append(ch, ahead_buffer);
}

void def_change() {
  currentClassName = 0;
}

char receiver[256];

void var_process(char ch, coc_mode_t *next_mode) {
  /*  if(isspace(ch)) {
  } else */ if(ch == '(') {
    *next_mode = CALL;

  } else if(isseparator(ch)) {
    if(strcmp(ahead_buffer, "return") != 0)
      fprintf(out, "%s->%s", receiver, ahead_buffer);
    else
      fprintf(out, "return (object_t*)");
    fprintf(out, "%c", ch);
    reset_buffer(ahead_buffer);
    *next_mode = BODY;
  } else
    append(ch, ahead_buffer);
}

void var_change() {
  if(strlen(ahead_buffer) == 0)
    strcpy(ahead_buffer, "self");
  strcpy(receiver, ahead_buffer);
  
  reset_buffer(ahead_buffer);
}

void call_change() {
  if(strcmp(receiver, "super") == 0)
    fprintf(out, "invokeSuper(%s, \"%s\", (object_t *)self ", currentClassName, ahead_buffer);
  else 
    fprintf(out, "invoke(\"%s\", (object_t *)%s ", ahead_buffer, receiver);

  reset_buffer(ahead_buffer);
}

void call_process(char ch, coc_mode_t *next_mode) {
  static int balance = 1;

  if(ch == '(') 
    balance++;
  else if(ch == ')') 
    balance--;

  if(balance==0) {
    if(strlen(ahead_buffer))
       fprintf(out, ", ");
    fprintf(out, "%s%c", ahead_buffer, ch);
    reset_buffer(ahead_buffer);
    *next_mode = BODY;
    balance = 1;

  } else
    append(ch, ahead_buffer);  
}

static int preproc_inNumber=0;

void preproc_change() {
  preproc_inNumber = 0;
}

void preproc_process(char ch, coc_mode_t *next_mode) {
  if(!preproc_inNumber) {
    append(ch, ahead_buffer);
    if(ch == ' ') {
      preproc_inNumber = 1;
      fprintf(out, "#%s", ahead_buffer);
      reset_buffer(ahead_buffer);
    }
  } else {
    if(isspace(ch)) {
      //      fprintf(out, "GOT(%d)", atoi(ahead_buffer));
      lineno = atoi(ahead_buffer);
      fprintf(out, "%d%c", lineno, ch);
      reset_buffer(ahead_buffer);
      *next_mode = BODY;
    }
    append(ch, ahead_buffer);
  }
  
}

void coc_filter(const char* source, const char* dest, 
		const char* includePaths) {
  coc_mode_t current_mode = BODY, last_mode = BODY;
  char *cmd = malloc(strlen(source) + strlen(includePaths) + strlen(CO_INCLUDE_DIR) + 256);
  sprintf(cmd, "cpp -DACAROS %s %s -include %s", source, includePaths,
    	  CO_INCLUDE_DIR "/co.h");
  //sprintf(cmd, "cpp %s", source);
  in = popen(cmd, "r");
  out = fopen(dest, "w");

  fprintf(out, "#include <stdarg.h> /** this file is generated by coc */\n");
  lineno = 0;
  while(!feof(in)) {
    int ch = fgetc(in);
    if(ch == -1) 
      break;
    if(ch == '\n')
      lineno++;
    last_mode = current_mode;
    states[current_mode].process((char)ch, &current_mode);
    if(current_mode != last_mode && states[current_mode].change)
      states[current_mode].change();
  }

  pclose(in);
  fclose(out);
}
