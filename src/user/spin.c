#include "user/syscall.h"

void pause(void)
{
	for (int i = 0; i < 100000000; i++);
}

int main(void)
{
	while (1) {
		sys_puts("I'am still alive\n");
		pause();
	}

	return 0;
}
