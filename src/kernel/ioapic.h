#ifndef __IO_APIC_H__
#define __IO_APIC_H__

#include <stdint.h>

// This register selects the IOAPIC Register to be read/written. The data is
// then read from or written to the selected register through the IOWIN Register.
#define IOREGSEL	0x00000000	// r/w

// This register is used to write to and read from the register selected by the IOREGSEL Register.
// Readability/writability is determined  by the IOAPIC register that is currently selected.
#define IOWIN		0x00000010	// r/w

// This register contains the 4-bit APIC ID. The ID serves as a physical name of the IOAPIC.
// All APIC devices using the APIC bus should have a unique APIC ID. The APIC bus arbitration ID
// for the I/O unit is also writtten during a write to the APICID Register (same data is loaded into both).
// This register must be programmed with the correct ID value before using the IOAPIC for message transmission.
// XXX: ID must be stored inside bits 24:27
#define IOAPICID	0x00000000	// r/w

// The IOAPIC Version Register identifies the APIC hardware version.
// Software can use this to provide compatibility between different
// APIC implementations and their versions. In addition, this register
// provides the maximum number of entries in the I/O Redirection Table
// XXX: bits 0:7 - APIC VERSION — RO
//      bits 16:23 - Maximum Redirection Entry — RO
#define IOAPICVER	0x00000001	// ro

// The APICARB Register contains the bus arbitration priority for the
// IOAPIC. This register is loaded when the IOAPIC ID Register is written.
// XXX: bits 24:27 - IOAPIC Identification — R/W.
#define IOAPICARB	0x00000002	// ro

// IOREDTBL[23:0] — I/O REDIRECTION TABLE REGISTERS (64bit entries)
#define IOREDTBL_BASE	0x00000010	// r/w

// TODO: struct for ioredtbl

#define IOAPIC_SELECT(reg) {						\
	uint32_t *__sel_addr = (uint32_t *)(IOAPIC_BASE + IOREGSEL);	\
	*__sel_addr = (uint32_t)reg;					\
}

#define IOAPIC_WRITE(reg, val) {			\
	uint32_t *__reg_addr;				\
							\
	IOAPIC_SELECT(reg);				\
							\
	__reg_addr = (uint32_t *)(IOAPIC_BASE + IOWIN);	\
	*__reg_addr = (uint32_t)val;			\
}

#define IOAPIC_READ(reg) ({				\
	uint32_t *__reg_addr;				\
							\
	IOAPIC_SELECT(reg);				\
							\
	__reg_addr = (uint32_t *)(IOAPIC_BASE + IOWIN);	\
	*__reg_addr;					\
})

#endif
