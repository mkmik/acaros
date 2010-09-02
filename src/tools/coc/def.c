#include "def.h"
#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *def_parse_className(const char* def) {
  char buffer[256];
  char* sep;

  strcpy(buffer, def);
  sep = strchr(buffer, '@');
  if(sep == 0) {
    printf("SYNTAX ERROR: @-class@method\n");
    exit(1);
  }
  sep[0] = 0;
  
  return strdup(buffer);
}

char *def_parse_methodName(const char* def) {
  char buffer[256];
  const char* sep;
  const char* end;

  sep = strchr(def, '@');
  if(sep == 0) {
    printf("SYNTAX ERROR: @-class@method\n");
    exit(1);
  }
  sep++;
  end = sep;
  while(*end != '(' || isspace(*end))
    end++;
  strncpy(buffer, sep, end-sep);
  buffer[end-sep] = 0;

  return strdup(buffer);
}

static int isptrdecl(char ch) {
  return ch == ' ' || ch == '*';
}

static int isidentifier(char ch) {
  return isalnum(ch) || ch == '_' || ch == '-';
}

const char* def_parse_arg(const char* arg, char* type, char* name) {
  const char* btype = arg;
  while(isidentifier(*arg++));
  while(isptrdecl(*arg))
    arg++;
  
  strncpy(type, btype, arg-btype);
  type[arg-btype] = 0;

  const char* bname = arg;
  while(isalnum(*arg))
    arg++;
  
  strncpy(name, bname, arg-bname);
  name[arg-bname] = 0;

  while(*arg != ',' && *arg != ')')
    arg++;

  if(*arg++ == ')')
    return 0;
  else {
    while(isspace(*arg))
      arg++;
    return arg;
  }
}
