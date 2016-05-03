#include "kernel/cpu.h"
#include "stdlib/assert.h"

// TODO: append cpu (via spinlock)
static struct cpu_map cpu_map[CPU_MAX_CNT];
static struct cpu_context context[CPU_MAX_CNT];

cpuid_t cpu_id_by_hardware_id(hardware_cpuid_t id)
{
	for (uint32_t i = 0; i < CPU_MAX_CNT; i++) {
		if (cpu_map[i].real_id == id)
			return i;
	}

	return CPU_ID_NONE;
}

cpuid_t cpu_get_id(void)
{
	return 0;
}

struct cpu_context *cpu_context_by_id(cpuid_t id)
{
	assert(id < CPU_MAX_CNT);
	return &context[id];
}

struct cpu_context *cpu_context(void)
{
	cpuid_t id = cpu_get_id();
	return cpu_context_by_id(id);
}
