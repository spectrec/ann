#include "user/syscall.h"

int main(void)
{
	while (1) {
		sys_yield();
	}

	return 0;
}
