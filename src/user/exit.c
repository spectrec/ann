#include "user/syscall.h"

int main(void)
{
	sys_puts("before exit\n");
	sys_exit(7);
	sys_puts("after exit\n");

	return 33;
}
