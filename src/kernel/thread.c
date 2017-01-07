#include "stdlib/assert.h"
#include "stdlib/string.h"

#include "kernel/asm.h"
#include "kernel/thread.h"

#include "kernel/misc/gdt.h"
#include "kernel/misc/util.h"

#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

#if LAB >= 8
// arguments are passed via `rdi', `rsi', `rdx' (see IA-32 calling conventions)
static void thread_foo(struct task *thread, thread_func_t foo, void *arg)
{
	assert(thread != NULL && foo != NULL);

	foo(arg);

	task_destroy(thread);

	// call schedule
	asm volatile ("int3");
}
#endif

/*
 * LAB8 Instruction:
 * 1. create new task
 * 2. allocate and map stack (hint: you can use `USER_STACK_TOP')
 * 3. pass function arguments via `rdi, rsi, rdx' (store `data' on new stack)
 * 4. setup segment registers
 * 5. setup instruction pointer and stack pointer
 */
// Don't override stack (don't use large `data')
struct task *thread_create(const char *name, thread_func_t foo, const uint8_t *data, size_t size)
{
	(void)name;
	(void)foo;
	(void)data;
	(void)size;

	return NULL;
}

// LAB8 Instruction: just change `state', so scheduler can run this thread
void thread_run(struct task *thread)
{
	assert(thread->state == TASK_STATE_DONT_RUN);
}
