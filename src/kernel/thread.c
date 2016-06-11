#include "stdlib/assert.h"
#include "stdlib/string.h"

#include "kernel/asm.h"
#include "kernel/thread.h"

#include "kernel/misc/gdt.h"
#include "kernel/misc/util.h"

#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

// arguments are passed via `rdi', `rsi', `rdx' (see IA-32 calling conventions)
static void thread_foo(struct task *thread, thread_func_t foo, void *arg)
{
	assert(thread != NULL && foo != NULL);

	foo(arg);

	task_destroy(thread);

	// call schedule
	asm volatile ("int3");
}

// Don't override stack (don't use large `data')
struct task *thread_create(const char *name, thread_func_t foo, const uint8_t *data, size_t size)
{
	struct page *stack;
	struct task *task;

	if ((task = task_new(name)) == NULL)
		goto cleanup;

	if ((stack = page_alloc()) == NULL) {
		terminal_printf("Can't create thread `%s': no memory for stack\n", name);
		goto cleanup;
	}
	// I use `USER_*' constants here just because I don't want to create
	// separete ones for threads.
	if (page_insert(task->pml4, stack, USER_STACK_TOP-PAGE_SIZE, PTE_U | PTE_W) != 0) {
		terminal_printf("Can't create thread `%s': page_insert(stack) failed\n", name);
		goto cleanup;
	}

	// prepare stack and arguments
	uint8_t *stack_top = (uint8_t *)USER_STACK_TOP;
	{
		uintptr_t cr3 = rcr3();
		lcr3(PADDR(task->pml4));

		if (data != NULL) {
			// pointers must be ptr aligned
			void *data_ptr = (void *)ROUND_DOWN((uintptr_t)(stack_top-size), sizeof(void *));

			memcpy(data_ptr, data, size);
			data = stack_top = data_ptr;
		}

		// return address
		stack_top -= sizeof(uintptr_t);
		*(uintptr_t *)stack_top = (uintptr_t)0;

		task->context.gprs.rdi = (uintptr_t)task;
		task->context.gprs.rsi = (uintptr_t)foo;
		task->context.gprs.rdx = (uintptr_t)data;

		// 1st and 2nd args are from kernel space (not from stack),
		// so we can save direct pointers

		lcr3(cr3);
	}

	task->context.cs = GD_KT;
	task->context.ds = GD_KD;
	task->context.es = GD_KD;
	task->context.ss = GD_KD;

	task->context.rip = (uintptr_t)thread_foo;
	task->context.rsp = (uintptr_t)stack_top;

	return task;

cleanup:
	if (task != NULL)
		task_destroy(task);

	return NULL;
}

void thread_run(struct task *thread)
{
	assert(thread->state == TASK_STATE_DONT_RUN);
	thread->state = TASK_STATE_READY;
}
