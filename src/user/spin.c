#include "user/syscall.h"

void pause(void)
{
	for (int i = 0; i < 100000000; i++);
}

int main(void)
{
	int pid = sys_fork();
	if (pid < 0) {
		sys_puts("can't fork");
		return -1;
	}

	for (int i = 0; i < 10; i++) {
		if (pid == 0) {
			sys_puts("I'am still alive (child)\n");
		} else {
			sys_puts("I'am still alive (parent)\n");
		}

		pause();
	}

	return 0;
}
