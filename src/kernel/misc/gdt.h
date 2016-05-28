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
#define TYPE_AVAILABLE_TSS	0x9

#define DPL_U (0x3 << 5)
#define DPL_S (0)

#define IDT_DPL_U (0x3)
#define IDT_DPL_S (0)

#define GDT_DPL_U (0x3)
#define GDT_DPL_S (0)

#define TSS_DPL_U (0x3)
#define TSS_DPL_S (0)

#ifdef __ASSEMBLER__

#define SEG(flags_, base_, limit_)					\
	.long ((limit_) & 0xffff) | (((base_) & 0xffff) << 16);		\
	.long (((base_) >> 16) & 0xff) | (((flags_) & 0xf0ff) << 8) |	\
		((limit_) & 0xf0000) | ((base_) & 0xff000000)

#else

#include <stdint.h>

struct tss {
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t io_map_base;
} __attribute__((packed));

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
struct descriptor64 {
	union {
		struct {
			uint32_t a;
			uint32_t b;
			uint32_t c;
			uint32_t d;
		} dwords;

		struct {
			uint16_t target_offset0;
			uint16_t target_selector;
			uint16_t ist: 2, reserved0: 6, type: 4, s: 1, dpl: 2, p: 1;
			uint16_t target_offset1;
			uint32_t target_offset2;
			uint32_t reserved1;
		} interrupt, trap, gate;

		struct {
			uint16_t segment_limit0;
			uint16_t base_address0;
			uint16_t base_address1: 8, type: 4, s: 1, dpl: 2, p: 1;
			uint16_t segment_limit1: 4, avl: 1, ign: 2, g: 1, base_address2: 8;
			uint32_t base_address3;
			uint32_t mbz;
		} tss;
	};
} __attribute__ ((packed));

#define GATE(sel_, off_, ist_, type_, dpl_) (struct descriptor64) {	\
	.gate = {							\
		.target_offset0 = ((off_) & 0xffff),			\
		.target_selector = (sel_),				\
		.ist = (ist_),						\
		.type = (type_),					\
		.s = 0,							\
		.dpl = (dpl_),						\
		.p = 1,							\
		.target_offset1 = (((off_) >> 16) & 0xffff),		\
		.target_offset2 = ((off_) >> 32),			\
	}								\
}

#define TRAP_GATE(sel_, off_, ist_, dpl_) GATE(sel_, ((uint64_t)off_), ist_, TYPE_TRAP_GATE, dpl_)
#define INTERRUPT_GATE(sel_, off_, ist_, dpl_) GATE(sel_, ((uint64_t)off_), ist_, TYPE_INTERRUPT_GATE, dpl_)

#define TSS(base_, limit_, type_, dpl_) (struct descriptor64) {	\
	.tss = {						\
		.segment_limit0 = ((limit_) & 0xffff),		\
		.base_address0 = ((base_) & 0xffff),		\
		.base_address1 = (((base_) >> 16) & 0xff),	\
		.type = (type_),				\
		.s = 0,						\
		.dpl = (dpl_),					\
		.p = 1,						\
		.segment_limit1 = (((limit_) >> 16) & 0xf),	\
		.avl = 0,					\
		.ign = 0,					\
		.g = 0,						\
		.base_address2 = (((base_) >> 24) & 0xff),	\
		.base_address3 = ((base_ >> 32) & 0xffffffff),	\
		.mbz = 0,					\
	}							\
}

#define SEGMENT_TSS(base, limit, type, dpl) TSS((uint64_t)base, (uint32_t)limit, type, dpl)

#endif // ! __ASSEMBLER__

#define GD_KT	(0x0008)
#define GD_KD	(0x0010)
#define GD_UT	(0x0018)
#define GD_UD	(0x0020)

// TSS descriptors are 128bit long.
#define GD_TSS	(0x0028)

#endif // __GDT_H__
