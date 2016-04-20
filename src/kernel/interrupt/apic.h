#ifndef __APIC_H__
#define __APIC_H__

#ifndef __ASSEMBLER__
# include <stdint.h>
#endif
#include <kernel/lib/memory/layout.h>

#define MSR_APIC_BASE	0x1b

// bootstrap processor
#define APIC_BASE_FLAG_BSP	(1 << 8)
// global enable/disable APIC flag
#define APIC_BASE_FLAG_ENABLE	(1 << 11)

// Info about uniq apic id
#define APIC_OFFSET_ID		0x00000020

// EOI-broadcast flag, max lvt entries, version
#define APIC_OFFSET_VERSION	0x00000030

// LVT CMCI Register - Specifies interrupt delivery when an
// overflow condition of corrected machine check error
// count reaching a threshold value occurred in a
// machine check bank supporting CMCI
#define APIC_OFFSET_LVT_CMCI	0x000002f0

// LVT Timer Register - Specifies interrupt delivery when the
// APIC timer signals an interrupt
#define APIC_OFFSET_LVT_TIMER	0x00000320

// LVT Thermal Monitor Register - Specifies interrupt delivery
// when the thermal sensor generates an interrupt
#define APIC_OFFSET_LVT_TERMAL	0x00000330

// LVT Performance Counter Register - Specifies interrupt
// delivery when a performance counter gene rates an
// interrupt on overflow
#define APIC_OFFSET_LVT_PERF	0x00000340

// LVT LINT0 Register - Specifies interrupt delivery when an
// interrupt is signaled at the LINT0 pin
#define APIC_OFFSET_LVT_LINT0	0x00000350

// LVT LINT1 Register - Specifies interrupt delivery when an
// interrupt is signaled at the LINT1 pin
#define APIC_OFFSET_LVT_LINT1	0x00000360

// LVT Error Register - Specifies interrupt delivery when the
// APIC detects an internal error
#define APIC_OFFSET_LVT_ERROR	0x00000370

#define APIC_DCR_DIV2		0b0000
#define APIC_DCR_DIV4		0b0001
#define APIC_DCR_DIV8		0b0010
#define APIC_DCR_DIV16		0b0011
#define APIC_DCR_DIV32		0b1000
#define APIC_DCR_DIV64		0b1001
#define APIC_DCR_DIV128		0b1010
#define APIC_DCR_NODIV		0b1011

// Divide Configuration Register, possible values:
#define APIC_OFFSET_DCR		0x000003e0

// Initial Count Register
#define APIC_OFFSET_ICR		0x00000380

// Current Count Register
#define APIC_OFFSET_CCR		0x00000390

// Spurious-Interrupt Vector Register
#define APIC_OFFSET_SVR		0x000000f0
#define APIC_SVR_ENABLE		(1 << 8)

// End of interrupt
#define APIC_OFFSET_EOI		0x000000b0

#define APIC_READ(reg_off) ({				\
	*(uint32_t *)(APIC_BASE + reg_off);		\
})

#define APIC_WRITE(reg_off, val) {			\
	*(uint32_t *)(APIC_BASE + reg_off) = val;	\
}

#endif
