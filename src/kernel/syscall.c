#include "task.h"
#include "kernel/kernel.h"
#include "kernel/syscall.h"

#include <syscall.h>
#include <console/terminal.h>

void syscall(struct task *task)
{
	enum syscall syscall = task->context.gprs.rax;
	int64_t ret = 0;

	switch (syscall) {
	case SYSCALL_PUTS:
		terminal_printf("%s", (char *)task->context.gprs.rbx);
		break;
	default:
		panic("unknown syscall `%u'\n", syscall);
	}

	task->context.gprs.rax = ret;
	task_run(task);
}
