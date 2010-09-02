#ifndef _DEF_H_
#define _DEF_H_

char *def_parse_className(const char* def);
char *def_parse_methodName(const char* def);

const char* def_parse_arg(const char* arg, char* type, char* name);

#endif
