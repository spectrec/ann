#include "kernel/lib/memory/layout.h"

#include "kernel/misc/gdt.h"
#include "kernel/interrupt/apic.h"
#include "kernel/interrupt/interrupt.h"

#define interrupt_handler_no_error_code(name, num)	\
	.globl name;					\
	.type name, @function;				\
	.align 4;					\
	name:						\
	pushq $0;					\
	pushq $(num);					\
	jmp interrupt_handler_common

#define interrupt_handler_with_error_code(name, num)	\
	.globl name;					\
	.type name, @function;				\
	.align 4;					\
	name:						\
	pushq $(num);					\
	jmp interrupt_handler_common

// exceptions
interrupt_handler_no_error_code(interrupt_handler_div_by_zero, INTERRUPT_VECTOR_DIV_BY_ZERO)
interrupt_handler_no_error_code(interrupt_handler_debug, INTERRUPT_VECTOR_DEBUG)
interrupt_handler_no_error_code(interrupt_handler_nmi, INTERRUPT_VECTOR_NMI)
interrupt_handler_no_error_code(interrupt_handler_breakpoint, INTERRUPT_VECTOR_BREAKPOINT)
interrupt_handler_no_error_code(interrupt_handler_overflow, INTERRUPT_VECTOR_OVERFLOW)
interrupt_handler_no_error_code(interrupt_handler_bound_range, INTERRUPT_VECTOR_BOUND_RANGE)
interrupt_handler_no_error_code(interrupt_handler_ivalid_opcode, INTERRUPT_VECTOR_IVALID_OPCODE)
interrupt_handler_no_error_code(interrupt_handler_device_not_available, INTERRUPT_VECTOR_DEVICE_NOT_AVAILABLE)
interrupt_handler_with_error_code(interrupt_handler_double_fault, INTERRUPT_VECTOR_DOUBLE_FAULT)
interrupt_handler_with_error_code(interrupt_handler_invalid_tss, INTERRUPT_VECTOR_INVALID_TSS)
interrupt_handler_with_error_code(interrupt_handler_segment_not_present, INTERRUPT_VECTOR_SEGMENT_NOT_PRESENT)
interrupt_handler_with_error_code(interrupt_handler_stack, INTERRUPT_VECTOR_STACK)
interrupt_handler_with_error_code(interrupt_handler_general_protection, INTERRUPT_VECTOR_GENERAL_PROTECTION)
interrupt_handler_with_error_code(interrupt_handler_page_fault, INTERRUPT_VECTOR_PAGE_FAULT)
interrupt_handler_no_error_code(interrupt_handler_x86_fp_instruction, INTERRUPT_VECTOR_X86_FP_INSTRUCTION)
interrupt_handler_with_error_code(interrupt_handler_alignment_check, INTERRUPT_VECTOR_ALIGNMENT_CHECK)
interrupt_handler_no_error_code(interrupt_handler_machine_check, INTERRUPT_VECTOR_MACHINE_CHECK)
interrupt_handler_no_error_code(interrupt_handler_simd_fp, INTERRUPT_VECTOR_SIMD_FP)
interrupt_handler_no_error_code(interrupt_handler_security_exception, INTERRUPT_VECTOR_SECURITY_EXCEPTION)

// interrupts
interrupt_handler_no_error_code(interrupt_handler_timer, INTERRUPT_VECTOR_TIMER)
interrupt_handler_no_error_code(interrupt_handler_keyboard, INTERRUPT_VECTOR_KEYBOARD)

// syscall
interrupt_handler_no_error_code(interrupt_handler_syscall, INTERRUPT_VECTOR_SYSCALL)

interrupt_handler_common:
	pushq $0x0 // reserve space for segment registers
	movw %ds, 0(%rsp)
	movw %es, 2(%rsp)
	movw %fs, 4(%rsp)
	movw %gs, 6(%rsp)

	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r11
	pushq %r10
	pushq %r9
	pushq %r8

	pushq %rbp
	pushq %rsi
	pushq %rdi
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %rax

	pushq %rax
	movw $GD_KD, %ax
	movw %ax, %ds
	movw %ax, %es
	popq %rax

	// Doesn't return
	call interrupt_handler
