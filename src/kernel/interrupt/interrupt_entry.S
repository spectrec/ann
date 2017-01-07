#include "kernel/lib/memory/layout.h"

#include "kernel/misc/gdt.h"
#include "kernel/interrupt/apic.h"
#include "kernel/interrupt/interrupt.h"

#define interrupt_handler_no_error_code(name, num)	\
	.globl name;					\
	.type name, @function;				\
	.align 4;					\
	name:						\
	pushq $0; /*instead of error code*/		\
	pushq $(num);					\
	jmp interrupt_handler_common

#define interrupt_handler_with_error_code(name, num)	\
	.globl name;					\
	.type name, @function;				\
	.align 4;					\
	name:						\
	pushq $(num);					\
	jmp interrupt_handler_common

// LAB4-5 Instruction: create interrupt and exception entry point (use macros above)

// LAB4 instruction:
// -- save task context on stack
// -- setup kernel segment registers (only data required)
// -- call interrupt handler
interrupt_handler_common:

