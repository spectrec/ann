#include "stdlib/string.h"
#include "stdlib/assert.h"

#include "kernel/lib/memory/map.h"
#include "kernel/lib/memory/layout.h"
#include "kernel/lib/console/terminal.h"

#include "kernel/asm.h"
#include "kernel/cpu.h"
#include "kernel/task.h"
#include "kernel/thread.h"
#include "kernel/monitor.h"
#include "kernel/loader/config.h"
#include "kernel/interrupt/interrupt.h"

void kernel_panic(const char *fmt, ...);
panic_t panic = kernel_panic;

void kernel_panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	terminal_vprintf(fmt, ap);
	va_end(ap);

	while (1) {
		/*do nothing*/;
	}
}

#if LAB >= 3
struct page32 {
	uint32_t ref;
	uint64_t links;
} __attribute__((packed));

// Loader prepared some interesting info for us. Let's process it.
void kernel_init_mmap(void)
{
	struct kernel_config *config = (struct kernel_config *)KERNEL_INFO;
	struct cpu_context *cpu = cpu_context();
	static struct mmap_state state;
	struct page32 *pages32;
	struct gdtr {
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed)) gdtr;

	// Convert physical addresses into virtual ones
	config->gdt.ptr = VADDR(config->gdt.ptr);
	config->pml4.ptr = VADDR(config->pml4.ptr);
	config->pages.ptr = VADDR(config->pages.ptr);

	cpu->pml4 = config->pml4.ptr;

	// Reinitialize state
	state.free = (struct mmap_free_pages){ NULL };
	state.pages_cnt = config->pages_cnt;
	state.pages = config->pages.ptr;
	mmap_init(&state);

	sgdt(gdtr);

	// We must reload gdt to avoid page fault when accessing it after
	// removing unneeded mappings
	gdtr.base = config->gdt.uintptr;
	asm volatile("lgdt (%0)" : : "p"(&gdtr));

	pages32 = (struct page32 *)config->pages.ptr;
	cpu->pml4[0] = 0; // remove unneeded mappings

	// Convert `page32' into 64-bit `page'. And rebuild free list
	uint32_t used_pages = 0;
	for (int64_t i = state.pages_cnt-1; i >= 0; i--) {
		// LAB3 code here
		(void)pages32;
	}

	terminal_printf("Pages stat: used: `%u', free: `%u'\n",
			used_pages, state.pages_cnt - used_pages);
}
#endif

#if LAB >= 8
// Just to demonstrate possible threads implementation
void kernel_thread(void *arg __attribute__((unused)))
{
	while (1) {
		// call schedule
		asm volatile("int3");
	}
}
#endif

void kernel_main(void)
{
	// Initialize bss
	extern uint8_t edata[], end[];
	memset(edata, 0, end - edata);

	bool debug = true;
	while (debug == true) {
		// wait, until someone change condition above
	}

	// Reset terminal
	terminal_init();

#if LAB >= 3
	// Initialize memory (process info prepared by loader)
	kernel_init_mmap();
#endif

#if LAB >= 7
	// show command line (need only when keyboard enabled)
	monitor_init();
#endif

#if LAB >= 4
	// Init interrupts and exceptions.
	interrupt_init();
#endif

#if LAB >= 6
	// Initialize tasks free list
	task_init();

	//TASK_STATIC_INITIALIZER(hello);

	//TASK_STATIC_INITIALIZER(read_kernel);
	//TASK_STATIC_INITIALIZER(read_unmap);
	//TASK_STATIC_INITIALIZER(write_kernel);
	//TASK_STATIC_INITIALIZER(write_unmap);

	//TASK_STATIC_INITIALIZER(yield);
	//TASK_STATIC_INITIALIZER(fork);
	TASK_STATIC_INITIALIZER(spin);
	//TASK_STATIC_INITIALIZER(exit);
#endif

#if LAB >= 8
	struct task *thread = thread_create("scheduler", kernel_thread, NULL, 0);
	if (thread == NULL)
		panic("can't create kernel thread");
	thread_run(thread);
#endif

#if LAB >= 7
	// Do it after creating tasks, because timer may
	// panic if no tasks found.
	interrupt_enable();
#endif

#if LAB >= 6
	schedule();
#else
	terminal_printf("kernel initialized, hang\n");
	while (1)
		/* do nothing */;
#endif
}
