#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

/** gcc optimized builtin */
int strcmp(const char *s1, const char *s2);
/** gcc optimized builtin */
int strncmp(const char *s1, const char *s2, size_t n);
/** gcc optimized builtin */
char *strcpy(char *dest, const char *str);
char *strncpy(char *dest, const char *src, size_t n);

/** locate substring. find needle in haystack, returning the first occourence
 * or null if not found. */
char *strstr(const char *haystack, const char *needle);

/** returns the length of null terminated string s */
size_t strlen(const char *s);

/** duplicates the string s by allocating and copying */
char *strdup(const char *s);

/** like strdup but copies at most n characters.
 * a terminating NULL character is always added (n+1) */
char *strndup(const char *s, size_t n);

/** find the first occourence of the character c in s */
char *strchr(const char *s, int c);
/** find the last occourence of the character c in s */
char *strrchr(const char *s, int c);

/** appends the string str to dest */
char *strcat(char *dest, const char *src);
/** like strcat but at most n characters are appended */
char *strncat(char *dest, const char *src, size_t n);

/** gcc optimized builtin */
void *memcpy (void *__restrict __dest,
	      __const void *__restrict __src, size_t __n);
/** gcc optimized builtin */
void *memmove (void *__dest, __const void *__src, size_t __n);
/** gcc optimized builtin */
void *memset (void *__s, int __c, size_t __n);


#endif
