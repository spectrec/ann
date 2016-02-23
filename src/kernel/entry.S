.section .text
.global _start
.type _start, @function
_start:
	/*
	movq $stack_top, %rsp
	movq $0x0, %rbp
	*/
	movl $stack_top, %esp
	movl $0x0, %ebp

	# This function should not return
	call kernel_main

	# Stop the CPU until the next interrupt arrives
	hlt
hang:
	jmp hang

# Allocate new stack (`just in case'), `@nobits' - to make `ld' happy
.section .bootstrap_stack @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
