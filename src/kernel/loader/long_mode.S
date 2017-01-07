#include "kernel/misc/gdt.h"
#include "kernel/lib/memory/layout.h"

.section .long_mode_asm
.global entry_long_mode_asm

.type entry_long_mode_asm, @function
entry_long_mode_asm:
	pushl %ebp
	movl %esp, %ebp

	// Reload cs, to switch from compatible mode into long mode
	ljmp $GD_KT, $reload_decriptors

.code64
reload_decriptors:
	movw $GD_KD, %bx
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %ss

long_mode:
#if LAB < 3
	addq $0x1, 0xb8000
	jmp long_mode
#endif

enter_kernel:
	movq 8(%ebp), %rax

	movq $KERNEL_STACK_TOP, %rsp
	jmp *%rax
