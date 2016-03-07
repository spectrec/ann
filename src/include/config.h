#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

union kernel_ptr {
	uint64_t uintptr;
	void *ptr;
};

struct kernel_config {
	union kernel_ptr pml4;

	union kernel_ptr pages;
	uint64_t pages_cnt;

	union kernel_ptr gdt;
};

#endif
