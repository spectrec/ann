#include "kernel/task.h"
#include "kernel/syscall.h"
#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/mmu.h"
#include "kernel/lib/memory/layout.h"

#include "stdlib/assert.h"
#include "stdlib/string.h"
#include "stdlib/syscall.h"

#include "kernel/lib/console/terminal.h"

// LAB5 Instruction:
// - find page, virtual address `va' belongs to
// - insert it into `dest->pml4' and `src->pml4' if needed
__attribute__((unused))
static int task_share_page(struct task *dest, struct task *src, void *va, unsigned perm)
{
	uintptr_t va_addr = (uintptr_t)va;
	struct page *p = NULL;

	(void)src;
	(void)dest;
	(void)perm;
	(void)va_addr;

	terminal_printf("share page %p (va: %p): refs: %d\n", p, va, p->ref);

	return 0;
}

// LAB5 Instruction:
// - create new task, copy context, setup return value
//
// - share pages:
// - check all entries inside pml4 before `USER_TOP'
// - check all entrins inside page directory pointer
// - check all entries inside page directory
// - check all entries inside page table and share if present
//
// - mark new task as `ready'
// - return new task id
__attribute__((unused))
static int sys_fork(struct task *task)
{
	(void)task;
	return -1;
}

// LAB5 Instruction:
// - implement `puts', `exit', `fork' and `yield' syscalls
// - you can get syscall number from `rax'
// - return value also should be passed via `rax'
void syscall(struct task *task)
{
	task_run(task);
}
