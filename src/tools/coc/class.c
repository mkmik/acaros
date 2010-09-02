#include "class.h"
#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



const char *class_parse_className(const char* decl) {
  char buffer[256];
  const char* end;

  if(strncmp(decl, "class", 5) != 0)
    return "missingClassKeyword";

  // skip "class"
  while(!isspace(*decl++));
  while(isspace(*decl))
    decl++;
  
  end = decl;
  while(isalnum(*end))
    end++;
  strncpy(buffer, decl, end-decl);
  buffer[end-decl] = 0;
  return strdup(buffer);
}

const char *class_parse_superclassName(const char* decl) {
  char buffer[256];
  const char* end;

  decl = strchr(decl, ':');
  if(!decl)
    return "Object";

  decl++;
  while(!isspace(*decl++));
  end = decl;
  while(!isspace(*end) && *end != '{')
    end++;
  strncpy(buffer, decl, end-decl);
  buffer[end-decl] = 0;
  return strdup(buffer);
}
