#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <interrupt.h>

enum syscall {
	SYSCALL_PUTS	= 0,

	SYSCALL_LAST
};

#endif
