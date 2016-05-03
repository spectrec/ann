#include "user/syscall.h"

int main(void)
{
	char child_buffer[] = "child: 0\n";
	char *child_ptr = child_buffer + 7; // points to `0'

	char parent_buffer[] = "parent: 0\n";
	char *parent_ptr = parent_buffer + 8; // points to `0'

	for (int i = 0; i < 3; i++) {
		int r = sys_fork();

		if (r == 0) {
			*child_ptr = '0' + i;
			sys_puts(child_buffer);

			return 1;
		} else if (r == -1) {
			sys_puts("can't fork\n");
			return -1;
		} else {
			*parent_ptr = 'a' + i;

			sys_puts(parent_buffer);
			sys_yield();
			sys_puts(parent_buffer);
		}
	}

	return 0;
}
