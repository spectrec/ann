#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#ifdef __USER__
# error "This file is for kernel internal use only"
#endif

#include "kernel/lib/memory/mmu.h"

#define VADDR(paddr_) ((void *)((uintptr_t)(paddr_) + VADDR_BASE))
#define PADDR(vaddr_) ((uintptr_t)(vaddr_) - VADDR_BASE)

#define KERNEL_STACK_TOP	KERNEL_BASE
#define KERNEL_STACK_SIZE	(PAGE_SIZE * 8)

#define VPT_SIZE		(NPDP_ENTRIES * NPD_ENTRIES * NPT_ENTRIES * PAGE_SIZE) // 512 GB
#define VPT			(KERNEL_STACK_TOP - KERNEL_STACK_SIZE - VPT_SIZE)

#define KERNEL_INFO		(VPT - PAGE_SIZE)

// Virtual address of the APIC base
#define APIC_BASE		(KERNEL_INFO - PAGE_SIZE)
#define IOAPIC_BASE		(APIC_BASE - PAGE_SIZE)

#define INTERRUPT_STACK_SIZE	(PAGE_SIZE * 4)
#define INTERRUPT_STACK_TOP	(IOAPIC_BASE)


// Physical address of the APIC base
#define APIC_BASE_PA	0xFEE00000
// Physical address of the IO APIC base
#define IOAPIC_BASE_PA	0xFEC00000

#define USER_TOP	0x00000a0000000000	// 10 TB
#define USER_STACK_TOP	0x000000000a000000

#endif
