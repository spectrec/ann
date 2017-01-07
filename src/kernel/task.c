#include "stdlib/assert.h"
#include "stdlib/string.h"

#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

#include "kernel/asm.h"
#include "kernel/cpu.h"
#include "kernel/task.h"
#include "kernel/misc/elf.h"
#include "kernel/misc/gdt.h"
#include "kernel/misc/util.h"
#include "kernel/loader/config.h"


static LIST_HEAD(task_free, task) free_tasks = LIST_HEAD_INITIALIZER(task_free);
static struct task tasks[TASK_MAX_CNT];
static task_id_t last_task_id;

void task_init(void)
{
	struct cpu_context *cpu = cpu_context();

	cpu->task = &cpu->self_task;
	memset(cpu->task, 0, sizeof(*cpu->task));

	// LAB6 Instruction: initialize tasks list, and mark them as free
}

void task_list(void)
{
	terminal_printf("task_id        name           owner\n");
	for (uint32_t i = 0; i < TASK_MAX_CNT; i++) {
		if (tasks[i].state != TASK_STATE_RUN &&
		    tasks[i].state != TASK_STATE_READY)
			continue;

		terminal_printf("  %d         %s          %s\n", tasks[i].id, tasks[i].name,
				(tasks[i].context.cs & GDT_DPL_U) == 0 ? "kernel" : "user");
	}
}

// LAB6 Instruction:
// - find and destroy task with id == `task_id' (check `tasks' list)
// - don't allow destroy kernel thread (check privelege level)
void task_kill(task_id_t task_id)
{
	terminal_printf("Can't kill task `%d': no such task\n", task_id);
}

struct task *task_new(const char *name)
{
	struct kernel_config *config = (struct kernel_config *)KERNEL_INFO;
	pml4e_t *kernel_pml4 = config->pml4.ptr;
	struct page *pml4_page;
	struct task *task;

	task = LIST_FIRST(&free_tasks);
	if (task == NULL) {
		terminal_printf("Can't create task `%s': no more free tasks\n", name);
		return NULL;
	}

	LIST_REMOVE(task, free_link);
	memset(task, 0, sizeof(*task));

	strncpy(task->name, name, sizeof(task->name));

	task->id = ++last_task_id;
	task->state = TASK_STATE_DONT_RUN;

	if ((pml4_page = page_alloc()) == NULL) {
		terminal_printf("Can't create task `%s': no memory for new pml4\n", name);
		return NULL;
	}
	page_incref(pml4_page);

	// LAB6 instruction:
	// - setup `task->pml4'
	// - clear it
	// - initialize kernel space part of `task->pml4'
	(void)kernel_pml4;

	return task;
}

void task_destroy(struct task *task)
{
	if (task->pml4 == NULL)
		// Nothing to do (possible when `task_new' failed)
		return;

	struct cpu_context *cpu = cpu_context();
	assert(task != &cpu->self_task);
	if (task == cpu->task)
		cpu->task = NULL;

	// We must be inside `task' address space. Because we use
	// virtual address to modify page table. This is needed to
	// avoid any problems when killing forked process.
	uint64_t old_cr3 = rcr3();
	if (old_cr3 != PADDR(task->pml4)) {
		lcr3(PADDR(task->pml4));
	}

	// remove all mapped pages from current task
	for (uint16_t i = 0; i <= PML4_IDX(USER_TOP); i++) {
		uintptr_t pdpe_pa = PML4E_ADDR(task->pml4[i]);

		if ((task->pml4[i] & PML4E_P) == 0)
			continue;

		pdpe_t *pdpe = VADDR(pdpe_pa);
		for (uint16_t j = 0; j < NPDP_ENTRIES; j++) {
			uintptr_t pde_pa = PDPE_ADDR(pdpe[j]);

			if ((pdpe[j] & PDPE_P) == 0)
				continue;

			pde_t *pde = VADDR(pde_pa);
			for (uint16_t k = 0; k < NPD_ENTRIES; k++) {
				uintptr_t pte_pa = PTE_ADDR(pde[k]);

				if ((pde[k] & PDE_P) == 0)
					continue;

				pte_t *pte = VADDR(pte_pa);
				for (uint16_t l = 0; l < NPT_ENTRIES; l++) {
					if ((pte[l] & PTE_P) == 0)
						continue;

					page_decref(pa2page(PTE_ADDR(pte[l])));
				}

				pde[k] = 0;
				page_decref(pa2page(pte_pa));
			}

			pdpe[j] = 0;
			page_decref(pa2page(pde_pa));
		}

		task->pml4[i] = 0;
		page_decref(pa2page(pdpe_pa));
	}

	// Reload cr3, because it may be reused after `page_decref'
	if (old_cr3 != PADDR(task->pml4)) {
		lcr3(old_cr3);
	} else {
		struct kernel_config *config = (struct kernel_config *)KERNEL_INFO;
		lcr3(PADDR(config->pml4.ptr));

		// Don't destroy kernel pml4
		assert(config->pml4.ptr != task->pml4);
	}

	page_decref(pa2page(PADDR(task->pml4)));
	task->pml4 = NULL;

	LIST_INSERT_HEAD(&free_tasks, task, free_link);
	task->state = TASK_STATE_FREE;

	terminal_printf("task [%d] has been destroyed\n", task->id);
}

// LAB6 Instruction:
// - allocate space (use `page_insert')
// - copy `binary + ph->p_offset' into `ph->p_va'
// - don't forget to initialize bss (diff between `memsz and filesz')
__attribute__((unused))
static int task_load_segment(struct task *task, const char *name,
			     uint8_t *binary, struct elf64_program_header *ph)
{
	uint64_t va = ROUND_DOWN(ph->p_va, PAGE_SIZE);
	uint64_t size = ROUND_UP(ph->p_memsz, PAGE_SIZE);

	(void)va;
	(void)size;
	(void)name;
	(void)task;
	(void)binary;

	return 0;
}

static int task_load(struct task *task, const char *name, uint8_t *binary, size_t size)
{
	struct elf64_header *elf_header = (struct elf64_header *)binary;

	if (elf_header->e_magic != ELF_MAGIC) {
		terminal_printf("Can't load task `%s': invalid elf magic\n", name);
		return -1;
	}

	// LAB6 Instruction:
	// - load all proram headers with type `load' (use `task_load_segment')
	// - setup task `rip'
	(void)task;
	(void)name;
	(void)size;

	return 0;
}

int task_create(const char *name, uint8_t *binary, size_t size)
{
	struct page *stack;
	struct task *task;

	if ((task = task_new(name)) == NULL)
		return -1;

	if (task_load(task, name, binary, size) != 0)
		goto cleanup;

	// LAB6 Instruction:
	// - allocate and map stack
	// - setup segment registers (with proper privelege levels) and stack pointer
	(void)stack;

	return 0;

cleanup:
	task_destroy(task);
	return -1;
}

void task_run(struct task *task)
{
#if LAB >= 7
	// Always enable interrupts
	task->context.rflags |= RFLAGS_IF;
#endif

	task->state = TASK_STATE_RUN;

	asm volatile(
		"movq %0, %%rsp\n\t"

		// restore gprs
		"popq %%rax\n\t"
		"popq %%rbx\n\t"
		"popq %%rcx\n\t"
		"popq %%rdx\n\t"
		"popq %%rdi\n\t"
		"popq %%rsi\n\t"
		"popq %%rbp\n\t"

		"popq %%r8\n\t"
		"popq %%r9\n\t"
		"popq %%r10\n\t"
		"popq %%r11\n\t"
		"popq %%r12\n\t"
		"popq %%r13\n\t"
		"popq %%r14\n\t"
		"popq %%r15\n\t"

		// restore segment registers (don't restore gs, fs - #GP will occur)
		"movw 0(%%rsp), %%ds\n\t"
		"movw 2(%%rsp), %%es\n\t"
		"addq $0x8, %%rsp\n\t"

		// skip interrupt_number and error_code
		"addq $0x10, %%rsp\n\t"

		"iretq" : : "g"(task) : "memory"
	);
}

void schedule(void)
{
	struct cpu_context *cpu = cpu_context();
	static int next_task_idx = 0;

	// LAB6 Instruction: implement scheduler
	// - check all tasks in state `ready'
	// - load new `cr3'
	// - setup `cpu' context
	// - run new task
	(void)cpu;
	(void)next_task_idx;

	panic("no more tasks");
}
