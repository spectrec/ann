#include "user/syscall.h"
#include "stdlib/string.h"

int main(void)
{
	char buffer[10];

	sys_puts("before read\n");
	memcpy(buffer, (void *)0x0, sizeof(buffer));
	sys_puts("after read");

	return 0;

}
