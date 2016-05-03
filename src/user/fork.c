#include "user/syscall.h"

int main(void)
{
	for (int i = 0; i < 5; i++) {
		int r = sys_fork();

		if (r == 0) {
			sys_puts("hello from child\n");
			return 1;
		} else if (r == -1) {
			sys_puts("can't fork\n");
		} else {
			sys_puts("hello from parent\n");
		}
	}

	return 0;
}
