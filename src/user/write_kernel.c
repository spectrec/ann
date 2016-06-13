#include "user/syscall.h"

#define KERNEL_BASE 0xFFFFFFF800000000

int main(void)
{
	char *kernel = (char *)KERNEL_BASE;

	sys_puts("before write\n");
	kernel[0] = 'x';
	sys_puts("after write");

	return 0;
}
