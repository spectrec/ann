#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#define INTERRUPT_VECTOR_SYSCALL 34

#ifndef __ASSEMBLER__
enum syscall {
	SYSCALL_PUTS	= 0,
	SYSCALL_EXIT	= 1,
	SYSCALL_FORK	= 2,
	SYSCALL_YIELD	= 3,

	SYSCALL_LAST
};
#endif

#endif
