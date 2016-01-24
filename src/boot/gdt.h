#ifndef __GDT_H__
#define __GDT_H__

// User segment flags
#define USF_G (1<<15)		// granularity: set limit in 4k blocks
#define USF_D (1<<14)		// default operand size: 32bit
#define USF_P (1<<7)		// segment present
#define USF_S (1<<4)		// user segment

// User segment type bits
#define UST_X (1<<3)		// code segment
#define UST_E (1<<2)		// segment is expand down (only data segments)
#define UST_C (1<<2)		// conforming bit (only code segment)
#define UST_W (1<<1)		// segment is writable (only data segment)
#define UST_R (1<<1)		// segment is readable (only code segment)
#define UST_A (1<<0)		// segment is accessed

#define DPL_U (0x3 << 5)
#define DPL_S (0)

#ifdef __ASSEMBLER__

#define SEG(flags_, base_, limit_)					\
	.long ((limit_) & 0xffff) | (((base_) & 0xffff) << 16);		\
	.long (((base_) >> 16) & 0xff) | (((flags_) & 0xf0ff) << 8) |	\
		((limit_) & 0xf0000) | ((base_) & 0xff000000)

#else

#include <stdint.h>

struct descriptor {
	union {
		struct {
			uint32_t a;
			uint32_t b;
		} dwords;

		struct {
			uint16_t limit0;
			uint16_t base0;
			uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
			uint16_t limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
		} segment;

		struct {
		} gate;

		struct {
		} tss;
	};
} __attribute__ ((packed));

// See amd64 documentation (vol 2: System Programming)
#define SEGMENT_DESC(flags_, base_, limit_) (struct descriptor) {		\
	.dwords = {								\
		.a = ((limit_) & 0xffff) | (((base_) & 0xffff) << 16),		\
		.b = (((base_) >> 16) & 0xff) | (((flags_) & 0xf0ff) << 8) |	\
			((limit_) & 0xf0000) | ((base_) & 0xff000000),		\
	}									\
}

#endif // __ASSEMBLER__

#endif // __GDT_H__
