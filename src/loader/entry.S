#include <mm/layout.h>

.section .text
.global _start

.type _start, @function
_start:
	movl $boot_stack_top, %esp
	movl $0x0, %ebp

	# This function should not return
	call loader_main

	# Stop the CPU until the next interrupt arrives
	hlt
hang:
	jmp hang

.data

# Allocate new stack (for loader and kernel)
.p2align PAGE_SHIFT

.global boot_stack
boot_stack:

.skip KERNEL_STACK_SIZE

.global boot_stack_top
boot_stack_top:
