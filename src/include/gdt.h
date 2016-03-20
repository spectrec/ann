#ifndef __GDT_H__
#define __GDT_H__

// User segment flags
#define USF_G (1<<15)		// granularity: set limit in 4k blocks
#define USF_D (1<<14)		// default operand size: 32bit
#define USF_L (1<<13)		// run processor in 64bit (long) mode
#define USF_P (1<<7)		// segment present
#define USF_S (1<<4)		// user segment

// User segment type bits
#define UST_X (1<<3)		// code segment
#define UST_E (1<<2)		// segment is expand down (only data segments)
#define UST_C (1<<2)		// conforming bit (only code segment)
#define UST_W (1<<1)		// segment is writable (only data segment)
#define UST_R (1<<1)		// segment is readable (only code segment)
#define UST_A (1<<0)		// segment is accessed

// Gate descriptor types
#define TYPE_CALL_GATE		0xc
#define TYPE_INTERRUPT_GATE	0xe
#define TYPE_TRAP_GATE		0xf

#define DPL_U (0x3 << 5)
#define DPL_S (0)

#define IDT_DPL_U (0x3)
#define IDT_DPL_S (0)

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
	};
} __attribute__ ((packed));

// See amd64 documentation (vol 2: System Programming)
#define SEGMENT_DESC(flags_, base_, limit_) (struct descriptor){		\
	.dwords = {								\
		.a = ((limit_) & 0xffff) | (((base_) & 0xffff) << 16),		\
		.b = (((base_) >> 16) & 0xff) | (((flags_) & 0xf0ff) << 8) |	\
			((limit_) & 0xf0000) | ((base_) & 0xff000000),		\
	}									\
}


// XXX: `s' must be zero
struct gate {
	union {
		struct {
			uint32_t a;
			uint32_t b;
			uint32_t c;
			uint32_t d;
		} dwords;

		struct {
			uint16_t limit0;
			uint16_t base0;
			uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
			uint16_t limit: 4, avl: 1, unused0: 2, g: 1, base2: 8;
			uint32_t base3;
			uint32_t reserved0: 8, zero: 5, reserved1 : 19;
		} call;

		struct {
			uint16_t target_offset0;
			uint16_t target_selector;
			uint16_t ist: 2, reserved0: 6, type: 4, s: 1, dpl: 2, p: 1;
			uint16_t target_offset1;
			uint32_t target_offset2;
			uint32_t reserved1;
		} interrupt, trap, gate;
	};
} __attribute__ ((packed));

#define GATE(sel_, off_, ist_, type_, dpl_) (struct gate) {	\
	.gate = {						\
		.target_offset0 = ((off_) & 0xffff),		\
		.target_selector = (sel_),			\
		.ist = (ist_),					\
		.type = (type_),				\
		.s = 0,						\
		.dpl = (dpl_),					\
		.p = 1,						\
		.target_offset1 = (((off_) >> 16) & 0xffff),	\
		.target_offset2 = ((off_) >> 32),		\
	}							\
}

#define TRAP_GATE(sel_, off_, ist_, dpl_) GATE(sel_, ((uint64_t)off_), ist_, TYPE_INTERRUPT_GATE, dpl_)
#define INTERRUPT_GATE(sel_, off_, ist_, dpl_) GATE(sel_, ((uint64_t)off_), ist_, TYPE_TRAP_GATE, dpl_)

#endif // ! __ASSEMBLER__

#define GD_KT	(sizeof(struct descriptor) << 0ul)
#define GD_KD	(sizeof(struct descriptor) << 1ul)
#define GD_UD	(sizeof(struct descriptor) << 2ul)
#define GD_UT	(sizeof(struct descriptor) << 3ul)
#define GD_TSS	(sizeof(struct descriptor) << 4ul)

#endif // __GDT_H__
