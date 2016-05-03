#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>

#include "task.h"

#define CPU_MAX_CNT	32
#define CPU_ID_NONE	-1

typedef uint32_t hardware_cpuid_t;
typedef uint16_t cpuid_t;

struct cpu_map {
	hardware_cpuid_t real_id;
	cpuid_t logic_id;
};

struct cpu_context {
	pml4e_t *pml4;

	struct task *task;
	struct task self_task;
};

cpuid_t cpu_get_id(void);
struct cpu_context *cpu_context(void);
struct cpu_context *cpu_context_by_id(cpuid_t id);

#endif
