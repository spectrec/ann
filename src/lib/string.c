#include <stdint.h>

#include "string.h"

// XXX: works only if (0 <= c && c <= 255)
void *memset(void *s, int c, size_t n)
{
	for (size_t i = 0; i < n; i++)
		*((uint8_t *)s + i) = (uint8_t)c;

	return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++)
		*((uint8_t *)dest + i) = *((uint8_t *)src + i);

	return dest;
}
