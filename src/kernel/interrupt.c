#include <gdt.h>
#include <x86.h>
#include <mmu.h>
#include <cpuid.h>
#include <mm/layout.h>
#include <console/terminal.h>

#include "pic.h"
#include "apic.h"
#include "kernel.h"
#include "ioapic.h"
#include "interrupt.h"

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

#define APIC_READ(reg_off) ({			\
	*(uint32_t *)(APIC_BASE + reg_off);	\
})

#define APIC_WRITE(reg_off, val) {			\
	*(uint32_t *)(APIC_BASE + reg_off) = val;	\
}

void interrupt_handler(void)
{
	terminal_printf("interrupt!\n");
}

int ioapic_init(void)
{
	uint32_t eax, ebx, ecx, edx;

	if (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0) {
		terminal_printf("cpuid failed\n");
		return -1;
	}
	terminal_printf("Local APIC ID: %x\n", (ebx >> 24) & 0xFF);

	uint32_t ver = IOAPIC_READ(IOAPICVER);
	terminal_printf("[IOAPIC] Maximum Redirection Entry: %u\n", (ver >> 16) + 1);

	// timer
	IOAPIC_WRITE(IOREDTBL_BASE, 0x20);
	IOAPIC_WRITE(IOREDTBL_BASE+1, ebx);

	// keyboard
	IOAPIC_WRITE(IOREDTBL_BASE+2, 0x21);
	IOAPIC_WRITE(IOREDTBL_BASE+3, ebx);

	return 0;
}

void apic_enable(void)
{
	// 0x1B - msr of local apic
	// bit 11 - global enable/disable APIC flag
	asm volatile(
		"movl $0x1b, %ecx\n\t"
		"rdmsr\n\t"
		"btsl $11, %eax\n\t"
		"wrmsr"
	);
}

static struct gate idt[256];
void interrupt_init(void)
{
	struct idtr {
		uint16_t limit;
		void *base;
	} __attribute__((packed)) idtr = { sizeof(idt), idt };

	extern void isr_wrapper(void);
	//for (uint32_t i = 0; i < 64; i++) {
	//	idt[i] = INTERRUPT_GATE(GD_KT, isr_wrapper, 0, IDT_DPL_S);
	//}
	idt[33] = INTERRUPT_GATE(GD_KT, isr_wrapper, 0, IDT_DPL_S);

	// Load idt
	asm volatile("lidt %0" :: "m" (idtr));

	// We are going to use IO APIC, so we must disable PIC.
	outb(PIC1_DATA, 0xff);
	outb(PIC2_DATA, 0xff);

	// First of all - enable apic
	apic_enable();

	if (ioapic_init() != 0)
		panic("ioapic_init failed\n");

	//APIC_WRITE(APIC_OFFSET_LVT_TIMER, (1ul << 17) | 0x20);
	//APIC_WRITE(APIC_OFFSET_DCR, 0b1001);
	//APIC_WRITE(APIC_OFFSET_ICR, 2083333);

	//APIC_WRITE(APIC_OFFSET_SVR, 0xff | APIC_SVR_ENABLE);

	asm volatile("sti");

	while (1) {
	}
}
