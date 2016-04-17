#ifndef __X86_H__
#define __X86_H__

#include <stdint.h>

#define RFLAGS_CF	(1 << 0) // Carry flag
// reserved		(1 << 1)
#define RFLAGS_PF	(1 << 2) // Parity flag
// reserved		(1 << 3)
#define RFLAGS_AF	(1 << 4) // Auxiliary flag
// reserved		(1 << 5)
#define RFLAGS_ZF	(1 << 6) // Zero flag
#define RFLAGS_SF	(1 << 7) // Sign flag
#define RFLAGS_TF	(1 << 8) // Trap flag
#define RFLAGS_IF	(1 << 9) // Interrupt flag
#define RFLAGS_DF	(1 << 10) // Direction flag
#define RFLAGS_OF	(1 << 11) // Overflow flag
#define RFLAGS_IOPL0	(1 << 12) // I/O Privilege level
#define RFLAGS_IOPL1	(1 << 13) // I/O Privilege level
#define RFLAGS_NT	(1 << 14) // Nested task
// reserved		(1 << 15)
#define RFLAGS_RF	(1 << 16) // Resume flag
#define RFLAGS_VM	(1 << 17) // Virtual-8086 mode
#define RFLAGS_AC	(1 << 18) // Alignment check
#define RFLAGS_VIF	(1 << 19) // Virtual interrupt flag
#define RFLAGS_VIP	(1 << 20) // Virtual interrupt pending
#define RFLAGS_ID	(1 << 21) // ID flag
// reserved		(1 << 22..63)

static inline uint8_t inb(int port)
{
	uint8_t data;
	__asm__ volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline uint16_t inw(int port)
{
	uint16_t data;
	__asm__ volatile("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outb(int port, uint8_t data)
{
	__asm__ volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static inline void ltr(uint16_t sel)
{
	__asm__ volatile("ltr %0" : : "r" (sel));
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

static inline void lcr3(uintptr_t val)
{
	__asm__ volatile("movq %0, %%cr3" : : "r" (val));
}

static inline uintptr_t rcr3(void)
{
	uintptr_t val;
	__asm__ volatile("movq %%cr3, %0" : "=r" (val));
	return val;
}

static inline uintptr_t rrsp(void)
{
	uintptr_t val;
	__asm__ volatile("movq %%rsp, %0" : "=r" (val));
	return val;
}

static inline uintptr_t rcr2(void)
{
	uintptr_t val;
	__asm__ volatile("movq %%cr2, %0" : "=r" (val));
	return val;
}

#define sgdt(gdtr) \
	__asm__ volatile("sgdt %0" : : "m"(gdtr) : "memory")

#endif
