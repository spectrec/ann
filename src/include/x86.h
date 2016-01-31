#ifndef __X86_H__
#define __X86_H__

#include <stdint.h>

static inline void insl(int port, void *addr, int cnt)
{
	__asm__ volatile("cld\n\trepne\n\tinsl"			:
			 "=D" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "memory", "cc");
}

static inline uint8_t inb(int port)
{
	uint8_t data;
	__asm__ volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outb(int port, uint8_t data)
{
	__asm__ volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

#endif
