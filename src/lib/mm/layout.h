#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include <mmu.h>

// Loader virtual address equal to physical one, so this value is `0'.
// Kernel must provide it's own `VADDR_BASE'
#ifndef VADDR_BASE
// Only for loader
# define VADDR_BASE	0
#else
// Only for kernel
# define KERNEL_BASE	VADDR_BASE
#endif

#define VADDR(paddr_) ((void *)((uintptr_t)(paddr_) + VADDR_BASE))
#define PADDR(vaddr_) ((uintptr_t)(vaddr_) - VADDR_BASE)

#define KERNEL_STACK_TOP	KERNEL_BASE
#define KERNEL_STACK_SIZE	(PAGE_SIZE * 8)

#define VPT_SIZE		(NPDP_ENTRIES * NPD_ENTRIES * NPT_ENTRIES * PAGE_SIZE) // 512 GB
#define VPT			(KERNEL_STACK_TOP - KERNEL_STACK_SIZE - VPT_SIZE)

#define KERNEL_INFO		(VPT - PAGE_SIZE)

#define INTERRUPT_STACK_SIZE	(PAGE_SIZE * 2)
#define INTERRUPT_STACK_TOP	(VPT)

// Physical address of the APIC base
#define APIC_BASE_PA	0xFEE00000
// Physical address of the IO APIC base
#define IOAPIC_BASE_PA	0xFEC00000

// Virtual address of the APIC base
#define APIC_BASE	(KERNEL_INFO - PAGE_SIZE)
#define IOAPIC_BASE	(APIC_BASE - PAGE_SIZE)

#endif
