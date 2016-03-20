#include <mm/layout.h>

#include "apic.h"

.globl isr_wrapper
.type isr_wrapper, @function
.align 4

isr_wrapper:
	call interrupt_handler

	// Send EOF
	/*
	pushq %rbx
	movl $(VADDR_BASE + APIC_BASE_PA + APIC_OFFSET_EOI), %ebx
	movl $0x0, (%ebx)
	popq %rbx
	*/

	iretq
