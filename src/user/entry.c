#include "user/syscall.h"

int main(void);

void user_entry(void)
{
	int ret = main();
	sys_exit(ret);
}
