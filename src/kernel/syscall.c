#include "kernel/task.h"
#include "kernel/syscall.h"
#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"

#include "stdlib/assert.h"
#include "stdlib/string.h"
#include "stdlib/syscall.h"

#include "kernel/lib/console/terminal.h"

static int task_dup_page(struct task *dest, struct task *src, void *va, uint8_t perm)
{
	uintptr_t va_addr = (uintptr_t)va;
	struct page *p = page_alloc();

	if (p == NULL) {
		terminal_printf("task_dup_page: can't allocate page\n");
		return -1;
	}

	if (page_insert(dest->pml4, p, va_addr, perm) != 0)
		return -1;
	if (page_insert(src->pml4, p, USER_TEMP, PTE_W) != 0)
		return -1;
	memcpy((void *)USER_TEMP, va, PAGE_SIZE);
	page_remove(src->pml4, USER_TEMP);

	return 0;
}

static int sys_fork(struct task *task)
{
	struct task *child = task_new();

	if (child == NULL)
		return -1;
	child->context = task->context;
	child->context.gprs.rax = 0; // return value

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

					uint8_t perm = pte[l] & PTE_FLAGS_MASK;
					if (task_dup_page(child, task, PAGE_ADDR(i, j, k, l, 0), perm) != 0) {
						task_destroy(child);
						return -1;
					}
				}
			}
		}
	}

	child->state = TASK_STATE_READY;

	return child->id;
}

void syscall(struct task *task)
{
	enum syscall syscall = task->context.gprs.rax;
	int64_t ret = 0;

	switch (syscall) {
	case SYSCALL_PUTS:
		terminal_printf("task [%d]: %s", task->id, (char *)task->context.gprs.rbx);
		break;
	case SYSCALL_EXIT:
		terminal_printf("task [%d] exited with value `%d'\n",
				task->id, task->context.gprs.rbx);
		task_destroy(task);

		return schedule();
	case SYSCALL_FORK:
		ret = sys_fork(task);
		break;
	case SYSCALL_YIELD:
		return schedule();
	default:
		panic("unknown syscall `%u'\n", syscall);
	}

	task->context.gprs.rax = ret;
	task_run(task);
}
