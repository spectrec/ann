#include "user/syscall.h"

#define MIDDLE	50000000
#define LONG	100000000

void pause(uint32_t period)
{
	for (uint32_t i = 0; i < period; i++);
}

int main(void)
{
	int pid = sys_fork();
	if (pid < 0) {
		sys_puts("can't fork");
		return -1;
	}

	if (pid == 0)
		pause(MIDDLE);

	for (uint32_t i = 0; i < 10; i++) {
		if (pid == 0) {
			sys_puts("I'am still alive (child)\n");
		} else {
			sys_puts("I'am still alive (parent)\n");
		}

		pause(LONG);
	}

	return 0;
}
