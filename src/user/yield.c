#include "user/syscall.h"

void pause(void)
{
	for (int i = 0; i < 100000000; i++);
}

int main(void)
{
	int pid;

	for (int i = 0; i < 2; i++) {
		if ((pid = sys_fork()) < 0) {
			sys_puts("can't fork\n");
			return -1;
		}

		if (pid == 0)
			// only parent should create childs
			break;
	}

	for (int i = 0; i < 10; i++) {
		sys_puts("before yield\n");
		sys_yield();
		sys_puts("after yield\n");
		pause();
	}

	return 0;
}
