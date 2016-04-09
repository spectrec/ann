#include "task.h"

void task_run(const struct task *task)
{
	asm volatile(
		"movq %0, %%rsp\n\t"

		// restore gprs
		"popq %%rax\n\t"
		"popq %%rbx\n\t"
		"popq %%rcx\n\t"
		"popq %%rdx\n\t"
		"popq %%rdi\n\t"
		"popq %%rsi\n\t"
		"popq %%rbp\n\t"
		"popq %%rsp\n\t"

		"popq %%r8\n\t"
		"popq %%r9\n\t"
		"popq %%r10\n\t"
		"popq %%r11\n\t"
		"popq %%r12\n\t"
		"popq %%r13\n\t"
		"popq %%r14\n\t"
		"popq %%r15\n\t"

		// restore segment registers (don't restore gs, fs - #GP will occur)
		"movw 0(%%rsp), %%ds\n\t"
		"movw 2(%%rsp), %%es\n\t"
		"addq $0x8, %%rsp\n\t"

		// skip interrupt_number and error_code
		"addq $0x10, %%rsp\n\t"

		"iretq" : : "g"(task) : "memory"
	);
}
