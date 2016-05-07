#include <stdint.h>

#include "string.h"

size_t strlen(const char *s)
{
	size_t r = 0;

	while (s[r] != '\0')
		r++;

	return r;
}

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

int strcmp(const char *s1, const char *s2)
{
	int r;

	while ((r = *s1 - *s2) == 0 && *s1 != '\0') {
		s1++, s2++;
	}

	return r;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		int r = *s1 - *s2;

		if (r != 0)
			return r;

		if (*s1 == '\0')
			return 0;

		s1++, s2++;
	}

	return 0;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		dest[i] = src[i];

		if (src[i] == '\0')
			break;
	}

	// just in case
	dest[n-1] = '\0';

	return dest;
}
