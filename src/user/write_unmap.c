#include "user/syscall.h"

int main(void)
{
	char *buf = (char *)0x0;

	sys_puts("before write\n");
	buf[0] = 'x';
	sys_puts("after write");

	return 0;
}
