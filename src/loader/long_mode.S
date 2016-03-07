#include <mm/layout.h>

.section .long_mode_asm
.global entry_long_mode_asm

.type entry_long_mode_asm, @function
entry_long_mode_asm:
	pushl %ebp
	movl %esp, %ebp

	// Enable/disable debug (1 - debug, 0 - nodebug)
	movl $0, %eax
	popl %edx

	// Reload cs, to switch from compatible mode into long mode
	ljmp $0x8, $long_mode

.code64
long_mode:
	cmpl $0, %eax
	je enter_kernel

	addq $0x1, 0xb8000
	jmp long_mode

enter_kernel:
	movq 8(%ebp), %rax

	movq $KERNEL_STACK_TOP, %rsp
	jmp *%rax
