#ifndef __THREAD_H__
#define __THREAD_H__

#include "kernel/task.h"

typedef void (*thread_func_t)(void *arg);

struct task *thread_create(const char *name, thread_func_t foo, const uint8_t *data, size_t size);
void thread_run(struct task *thread);

#endif
