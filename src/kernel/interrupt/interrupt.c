#include <cpuid.h>

#include "stdlib/assert.h"

#include "kernel/lib/memory/mmu.h"
#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

#include "kernel/asm.h"
#include "kernel/task.h"
#include "kernel/syscall.h"
#include "kernel/misc/tss.h"
#include "kernel/misc/gdt.h"
#include "kernel/loader/config.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/interrupt/pic.h"
#include "kernel/interrupt/apic.h"
#include "kernel/interrupt/ioapic.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/interrupt/timer.h"
#include "kernel/interrupt/keyboard.h"

// interrupt handler entry points
void interrupt_handler_div_by_zero();
void interrupt_handler_debug();
void interrupt_handler_nmi();
void interrupt_handler_breakpoint();
void interrupt_handler_overflow();
void interrupt_handler_bound_range();
void interrupt_handler_ivalid_opcode();
void interrupt_handler_device_not_available();
void interrupt_handler_double_fault();
void interrupt_handler_invalid_tss();
void interrupt_handler_segment_not_present();
void interrupt_handler_stack();
void interrupt_handler_general_protection();
void interrupt_handler_page_fault();
void interrupt_handler_x86_fp_instruction();
void interrupt_handler_alignment_check();
void interrupt_handler_machine_check();
void interrupt_handler_simd_fp();
void interrupt_handler_security_exception();
void interrupt_handler_timer();
void interrupt_handler_keyboard();
void interrupt_handler_syscall();

static struct descriptor64 idt[256];

static struct tss tss[CPU_MAX_CNT]
	__attribute__((aligned(PAGE_SIZE)));

static const char *interrupt_name[256] = {
	[INTERRUPT_VECTOR_DIV_BY_ZERO] = "divide by zero",
	[INTERRUPT_VECTOR_DEBUG] = "debug",
	[INTERRUPT_VECTOR_NMI] = "nmi",
	[INTERRUPT_VECTOR_BREAKPOINT] = "breakpoint",
	[INTERRUPT_VECTOR_OVERFLOW] = "overflow",
	[INTERRUPT_VECTOR_BOUND_RANGE] = "bound range",
	[INTERRUPT_VECTOR_IVALID_OPCODE] = "invalid opcode",
	[INTERRUPT_VECTOR_DEVICE_NOT_AVAILABLE] = "device not available",
	[INTERRUPT_VECTOR_DOUBLE_FAULT] = "double fault",
	[INTERRUPT_VECTOR_INVALID_TSS] = "invalid tss",
	[INTERRUPT_VECTOR_SEGMENT_NOT_PRESENT] = "segment not present",
	[INTERRUPT_VECTOR_STACK] = "stack",
	[INTERRUPT_VECTOR_GENERAL_PROTECTION] = "general protection",
	[INTERRUPT_VECTOR_PAGE_FAULT] = "page fault",
	[INTERRUPT_VECTOR_X86_FP_INSTRUCTION] = "x86 floating point instruction",
	[INTERRUPT_VECTOR_ALIGNMENT_CHECK] = "alignment check",
	[INTERRUPT_VECTOR_MACHINE_CHECK] = "machine check",
	[INTERRUPT_VECTOR_SIMD_FP] = "simd floating point",
	[INTERRUPT_VECTOR_SECURITY_EXCEPTION] = "security exception",
	[INTERRUPT_VECTOR_TIMER] = "timer",
	[INTERRUPT_VECTOR_KEYBOARD] = "keyboard",
	[INTERRUPT_VECTOR_SYSCALL] = "syscall",
};

#define PAGE_FAULT_ERROR_CODE_P		(1 << 0)
#define PAGE_FAULT_ERROR_CODE_R_W	(1 << 1)
#define PAGE_FAULT_ERROR_CODE_U_S	(1 << 2)
#define PAGE_FAULT_ERROR_CODE_RSV	(1 << 3)
#define PAGE_FAULT_ERROR_CODE_I_D	(1 << 4)
void page_fault_handler(struct task_context ctx)
{
	terminal_printf("Page fault at `%lx', opration: %s, accessed by: %s\n", rcr2(),
			(ctx.error_code & PAGE_FAULT_ERROR_CODE_R_W) != 0 ? "write" : "read",
			(ctx.error_code & PAGE_FAULT_ERROR_CODE_U_S) != 0 ? "user" : "supervisor");
	if ((ctx.error_code & PAGE_FAULT_ERROR_CODE_P) == 0)
		terminal_printf("\tpage is not present\n");
	if ((ctx.error_code & PAGE_FAULT_ERROR_CODE_RSV) != 0)
		terminal_printf("\tcheck reserved fields inside page tables\n");
	if ((ctx.error_code & PAGE_FAULT_ERROR_CODE_I_D) != 0)
		terminal_printf("\tfault was because of instruction fetch\n");

	// TODO: try to fix page fault
}

void interrupt_handler(struct task_context ctx)
{
	struct cpu_context *cpu = cpu_context();

	cpu->task.context = ctx;

	switch (ctx.interrupt_number) {
	case INTERRUPT_VECTOR_BREAKPOINT:
		terminal_printf("breakpoint\n");
		return task_run(&cpu->task);
	case INTERRUPT_VECTOR_PAGE_FAULT:
		return page_fault_handler(ctx);
	case INTERRUPT_VECTOR_DIV_BY_ZERO:
	case INTERRUPT_VECTOR_DEBUG:
	case INTERRUPT_VECTOR_NMI:
	case INTERRUPT_VECTOR_OVERFLOW:
	case INTERRUPT_VECTOR_BOUND_RANGE:
	case INTERRUPT_VECTOR_IVALID_OPCODE:
	case INTERRUPT_VECTOR_DEVICE_NOT_AVAILABLE:
	case INTERRUPT_VECTOR_DOUBLE_FAULT:
	case INTERRUPT_VECTOR_INVALID_TSS:
	case INTERRUPT_VECTOR_SEGMENT_NOT_PRESENT:
	case INTERRUPT_VECTOR_STACK:
	case INTERRUPT_VECTOR_GENERAL_PROTECTION:
	case INTERRUPT_VECTOR_X86_FP_INSTRUCTION:
	case INTERRUPT_VECTOR_ALIGNMENT_CHECK:
	case INTERRUPT_VECTOR_MACHINE_CHECK:
	case INTERRUPT_VECTOR_SIMD_FP:
	case INTERRUPT_VECTOR_SECURITY_EXCEPTION:
		break;
	case INTERRUPT_VECTOR_SYSCALL:
		return syscall(&cpu->task);
	case INTERRUPT_VECTOR_TIMER:
		return timer_handler(&cpu->task);
	case INTERRUPT_VECTOR_KEYBOARD:
		return keyboard_handler(&cpu->task);
	}

	terminal_printf("\nunhandled interrupt: %s (%u)\n",
			interrupt_name[ctx.interrupt_number],
			(uint32_t)ctx.interrupt_number);
	terminal_printf("Task dump:\n"
			"\trax: %lx, rbx: %lx, rcx: %lx, rdx: %lx\n"
			"\trdi: %lx, rsi: %lx, rsp: %lx\n"
			"\tr8:  %lx, r9:  %lx, r10: %lx, r11: %lx\n"
			"\tr12: %lx, r13: %lx, r14: %lx, r15: %lx\n"
			"\tcs: %x, ss: %x, ds: %x, es: %x, fs: %x, gs: %x\n"
			"\trip: %lx, rfalgs: %lb\n", ctx.gprs.rax, ctx.gprs.rbx,
			ctx.gprs.rcx, ctx.gprs.rdx, ctx.gprs.rdi, ctx.gprs.rsi,
			ctx.rsp, ctx.gprs.r8, ctx.gprs.r9,
			ctx.gprs.r10, ctx.gprs.r11, ctx.gprs.r12, ctx.gprs.r13,
			ctx.gprs.r14, ctx.gprs.r15, (uint32_t)ctx.cs, (uint32_t)ctx.ss,
			(uint32_t)ctx.ds, (uint32_t)ctx.es, (uint32_t)ctx.fs, (uint32_t)ctx.gs,
			ctx.rip, ctx.rflags);

	task_destroy(&cpu->task);
	schedule();
}

int ioapic_init(void)
{
	uint32_t eax, ebx, ecx, edx;

	if (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0) {
		terminal_printf("cpuid failed\n");
		return -1;
	}

	uint32_t local_apic_id = ebx;
	terminal_printf("Local APIC ID: %x\n", (local_apic_id >> 24) & 0xFF);

	uint32_t ver = IOAPIC_READ(IOAPICVER);
	terminal_printf("[IOAPIC] Maximum Redirection Entry: %u\n", (ver >> 16) + 1);

	// timer
	IOAPIC_WRITE(IOREDTBL_BASE, INTERRUPT_VECTOR_TIMER);
	IOAPIC_WRITE(IOREDTBL_BASE+1, local_apic_id);

	// keyboard
	IOAPIC_WRITE(IOREDTBL_BASE+2, INTERRUPT_VECTOR_KEYBOARD);
	IOAPIC_WRITE(IOREDTBL_BASE+3, local_apic_id);

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

void interrupt_init(void)
{
	struct kernel_config *config = (struct kernel_config *)KERNEL_INFO;
	struct descriptor *gdt = config->gdt.ptr;
	struct cpu_context *cpu = cpu_context();
	struct idtr {
		uint16_t limit;
		void *base;
	} __attribute__((packed)) idtr = {
		sizeof(idt)-1, idt
	};

	// setup interrupt handlers
	idt[INTERRUPT_VECTOR_DIV_BY_ZERO] = TRAP_GATE(GD_KT, interrupt_handler_div_by_zero, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_DEBUG] = TRAP_GATE(GD_KT, interrupt_handler_debug, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_NMI] = TRAP_GATE(GD_KT, interrupt_handler_nmi, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_BREAKPOINT] = TRAP_GATE(GD_KT, interrupt_handler_breakpoint, 0, IDT_DPL_U);
	idt[INTERRUPT_VECTOR_OVERFLOW] = TRAP_GATE(GD_KT, interrupt_handler_overflow, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_BOUND_RANGE] = TRAP_GATE(GD_KT, interrupt_handler_bound_range, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_IVALID_OPCODE] = TRAP_GATE(GD_KT, interrupt_handler_ivalid_opcode, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_DEVICE_NOT_AVAILABLE] = TRAP_GATE(GD_KT, interrupt_handler_device_not_available, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_DOUBLE_FAULT] = TRAP_GATE(GD_KT, interrupt_handler_double_fault, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_INVALID_TSS] = TRAP_GATE(GD_KT, interrupt_handler_invalid_tss, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_SEGMENT_NOT_PRESENT] = TRAP_GATE(GD_KT, interrupt_handler_segment_not_present, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_STACK] = TRAP_GATE(GD_KT, interrupt_handler_stack, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_GENERAL_PROTECTION] = TRAP_GATE(GD_KT, interrupt_handler_general_protection, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_PAGE_FAULT] = TRAP_GATE(GD_KT, interrupt_handler_page_fault, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_X86_FP_INSTRUCTION] = TRAP_GATE(GD_KT, interrupt_handler_x86_fp_instruction, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_ALIGNMENT_CHECK] = TRAP_GATE(GD_KT, interrupt_handler_alignment_check, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_MACHINE_CHECK] = TRAP_GATE(GD_KT, interrupt_handler_machine_check, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_SIMD_FP] = TRAP_GATE(GD_KT, interrupt_handler_simd_fp, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_SECURITY_EXCEPTION] = TRAP_GATE(GD_KT, interrupt_handler_security_exception, 0, IDT_DPL_S);

	// hardware interrups
	idt[INTERRUPT_VECTOR_TIMER] = INTERRUPT_GATE(GD_KT, interrupt_handler_timer, 0, IDT_DPL_S);
	idt[INTERRUPT_VECTOR_KEYBOARD] = INTERRUPT_GATE(GD_KT, interrupt_handler_keyboard, 0, IDT_DPL_S);

	// software interrupts
	idt[INTERRUPT_VECTOR_SYSCALL] = TRAP_GATE(GD_KT, interrupt_handler_syscall, 0, IDT_DPL_U);

	// Load idt
	asm volatile("lidt %0" :: "m" (idtr));

	// Initialize tss
	for (uint32_t idx = (GD_TSS >> 3), j = 0; j < CPU_MAX_CNT; idx += 2, j++) {
		struct descriptor64 *gdt64_entry = (struct descriptor64 *)&gdt[idx];
		*gdt64_entry = SEGMENT_TSS(&tss[j], sizeof(tss[j]), TYPE_AVAILABLE_TSS, TSS_DPL_S);
	}

	// Prepare stack for interrupts
	for (uintptr_t addr = INTERRUPT_STACK_TOP - INTERRUPT_STACK_SIZE;
	     addr < INTERRUPT_STACK_TOP; addr += PAGE_SIZE) {
		struct page *page;

		if ((page = page_alloc()) == NULL)
			panic("not enough memory for interrup handler stack");
		if (page_insert(cpu->pml4, page, addr, PTE_W) != 0)
			panic("can't map stack for interrupt handler");
	}

	// For now this os support only one processor, so we must initialize
	// only one tss. If you want use more processors -- you should
	// initialize tss for each one.
	tss[0].rsp0 = INTERRUPT_STACK_TOP;
	ltr(GD_TSS);

	// We are going to use IO APIC, so we must disable PIC.
	outb(PIC1_DATA, 0xff);
	outb(PIC2_DATA, 0xff);

	// First of all - enable apic
	apic_enable();

	if (ioapic_init() != 0)
		panic("ioapic_init failed");

	if (keyboard_init() != 0)
		panic("keyboard_init failed");

	if (timer_init() != 0)
		panic("timer_init failed");

	asm volatile("int3");
}
