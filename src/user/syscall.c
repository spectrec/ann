#include <stdint.h>

#include "stdlib/syscall.h"

static int64_t syscall(enum syscall syscall, uint64_t arg1, uint64_t arg2,
		       uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
	int64_t ret;

	asm volatile("int %1\n"
		: "=a" (ret)
		: "i" (INTERRUPT_VECTOR_SYSCALL),
		  "a" (syscall),
		  "b" (arg1),
		  "c" (arg2),
		  "d" (arg3),
		  "D" (arg4),
		  "S" (arg5)
		: "cc", "memory");

	return ret;
}

void sys_puts(const char *string)
{
	return (void)syscall(SYSCALL_PUTS, (uintptr_t)string, 0, 0, 0, 0);
}

void sys_exit(int ret)
{
	return (void)syscall(SYSCALL_EXIT, (int64_t)ret, 0, 0, 0, 0);
}

int sys_fork(void)
{
	return syscall(SYSCALL_FORK, 0, 0, 0, 0, 0);
}

void sys_yield(void)
{
	return (void)syscall(SYSCALL_YIELD, 0, 0, 0, 0, 0);
}
