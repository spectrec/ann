#include "user/syscall.h"
#include "stdlib/string.h"

#define KERNEL_BASE 0xFFFFFFF800000000

int main(void)
{
	char *kernel = (char *)KERNEL_BASE;
	char buffer[10];

	sys_puts("before read\n");
	memcpy(buffer, kernel, sizeof(buffer));
	sys_puts("after read");

	return 0;
}
