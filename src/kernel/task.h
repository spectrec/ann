#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

struct gprs {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rsp;

	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
};

struct task_context {
	struct gprs gprs; // general purpose registers

	uint64_t ds: 16;
	uint64_t es: 16;
	uint64_t fs: 16;
	uint64_t gs: 16;

	uint64_t interrupt_number;

	// All fields below are pushed by hardware
	uint64_t error_code: 32;
	uint64_t padding2: 32;

	uint64_t rip;

	uint64_t cs: 16;
	uint64_t padding1: 48;

	uint64_t rflags;
	uint64_t rsp;

	uint64_t ss: 16;
	uint64_t padding0: 48;
};

struct task {
	struct task_context context;
};

void task_run(const struct task *task);

#endif
