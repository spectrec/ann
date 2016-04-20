#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

size_t strlen(const char *s);

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);

#endif
