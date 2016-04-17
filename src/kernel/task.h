#ifndef __TASK_H__
#define __TASK_H__

#include <mmu.h>
#include <queue.h>
#include <stdint.h>
#include <stddef.h>

struct gprs {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;

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


enum task_state {
	TASK_STATE_FREE		= 0,
	TASK_STATE_READY	= 1,
	TASK_STATE_RUN		= 2,
	TASK_STATE_DONT_RUN	= 3,
};

typedef uint32_t task_id_t;

struct task {
	struct task_context context;
	enum task_state state;

	task_id_t id;

	LIST_ENTRY(task) free_link;

	pml4e_t *pml4; // virtual address of pml4
	uint64_t cr3; // physical address of pml4
};

void task_init(void);

struct task *task_new(void);
void task_destroy(struct task *task);
int task_create(const char *name, uint8_t *binary, size_t size);

void task_run(struct task *task);
void schedule(void);

#define TASK_MAX_CNT	1024

#define TASK_STATIC_INITIALIZER(name_) {				\
	extern uint8_t _binary_obj_user_## name_ ##_bin_start[];	\
	extern uint8_t _binary_obj_user_## name_ ##_bin_end[];		\
									\
	uint8_t *binary_ = _binary_obj_user_## name_ ##_bin_start;	\
	uint8_t *end_ = _binary_obj_user_## name_ ##_bin_end;		\
									\
	task_create(#name_, binary_, end_ - binary_);			\
}

#endif
