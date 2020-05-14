#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t num);
void *memmove(void *dest, const void *src, size_t num);
void *memset(void *ptr, int value, size_t num);
int memcmp (const void *m1, const void *m2, size_t n);

size_t strlen(const char *str);
int strcmp (const char *s1, const char *s2);
char *strstr(const char *hs, const char *ne);

#endif /* __STRING_H__ */
