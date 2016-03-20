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

static inline void invlpg(void *m)
{
	/* Clobber memory to avoid optimizer re-ordering access before invlpg,
	 * which may cause nasty bugs. */
	__asm__ volatile("invlpg (%0)" : : "b" (m) : "memory");
}

// Write a 64-bit value to a MSR. The A constraint stands
// for concatenation of registers EAX and EDX.
static inline void wrmsr(uint32_t msr_id, uint64_t value)
{
	__asm__ volatile("wrmsr" : : "c" (msr_id), "A" (value));
}

// Read a 64-bit value from a MSR. The A constraint stands
// for concatenation of registers EAX and EDX.
static inline uint64_t rdmsr(uint32_t msr_id)
{
	uint64_t value;
	__asm__ volatile("rdmsr" : "=A" (value) : "c" (msr_id));
	return value;
}

#endif
